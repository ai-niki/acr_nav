// Copyright (C) 2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: acr_nav (exe) -- TUI schema explorer for browsing ctypes, fields, and cross-references
// Exceptions: yes
// Source: cpp/acr_nav/main.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"
#include <termios.h>
#include <poll.h>
#include <signal.h>

using acr_nav::WriteStdout;
using acr_nav::DetectTerminal;
using acr_nav::SelectedCtype;
using acr_nav::ClearContentCaches;
using acr_nav::AdjustScroll;
using acr_nav::RightPanelItemCount;
using acr_nav::DataRows;
using acr_nav::BuildLeftItemsReset;
using acr_nav::BuildLeftItems;
using acr_nav::PopOverlay;
using acr_nav::DismissStartupHelp;
using acr_nav::PopOverlayOnCtypeChange;
using acr_nav::PushOverlay;
using acr_nav::GraphFindCtypeLine;
using acr_nav::SwitchToBrowse;
using acr_nav::BuildHelpLines;
using acr_nav::Render;
using acr_nav::RightPanelFieldFind;
using acr_nav::FieldMatchesFilter;
using acr_nav::RightPanelLineFind;
using acr_nav::BuildBreadcrumb;
using acr_nav::BuildStatusHint;
using acr_nav::FindSsimfile;
using acr_nav::GoToCtype;
using acr_nav::ToggleViewmode;

static struct termios acr_nav_orig_termios;
static bool acr_nav_raw_mode = false;
static volatile sig_atomic_t acr_nav_sigwinch = 0;

// Non-blocking key parser state for TUI+IPC mode
static char acr_nav_keybuf[4];
static int acr_nav_keybuf_n = 0;
static algo_lib::FTimehook acr_nav_esc_timer;
static bool acr_nav_esc_timer_armed = false;
static algo_lib::FTimehook acr_nav_sigwinch_timer;

static void SigwinchHandler(int) {
    acr_nav_sigwinch = 1;
}

// -----------------------------------------------------------------------------

static void ExitRawMode() {
    if (acr_nav_raw_mode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &acr_nav_orig_termios);
        acr_nav_raw_mode = false;
        WriteStdout("\x1b[2J\x1b[H\x1b[?25h\x1b[0m", 17);
    }
}

// -----------------------------------------------------------------------------

static void EnterRawMode() {
    tcgetattr(STDIN_FILENO, &acr_nav_orig_termios);
    atexit(ExitRawMode);
    struct termios raw = acr_nav_orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    acr_nav_raw_mode = true;
    WriteStdout("\x1b[2J\x1b[H\x1b[?25l", 13);
}

// -----------------------------------------------------------------------------

static bool ByteAvailable() {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    return poll(&pfd, 1, 50) > 0;
}

// -----------------------------------------------------------------------------

// VT100 byte-to-keyname mapping. Looks like a missing noun but
// these are terminal protocol constants, not application data.
// The data-driven boundary is one layer up: acr_navdb.keybind.
static tempstr ReadKeyName() {
    tempstr ret;
    char c = 0;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0 && errno == EINTR) {
        // interrupted by signal (e.g. SIGWINCH) — return empty key, caller repaints
    } else if (n <= 0) {
        acr_nav::_db.running = false;
    } else if (c == 3) {
        acr_nav::_db.running = false;
    } else if (c == '\x1b') {
        if (!ByteAvailable()) {
            ret = "Escape";
        } else {
            char seq[4];
            ssize_t nr;
            seq[0] = 0;
            seq[1] = 0;
            nr = read(STDIN_FILENO, &seq[0], 1);
            (void)nr;
            if (seq[0] == '[') {
                nr = read(STDIN_FILENO, &seq[1], 1);
                (void)nr;
                if (seq[1] == 'A') {
                    ret = "Up";
                } else if (seq[1] == 'B') {
                    ret = "Down";
                } else if (seq[1] == 'C') {
                    ret = "Right";
                } else if (seq[1] == 'D') {
                    ret = "Left";
                } else if (seq[1] == '5') {
                    nr = read(STDIN_FILENO, &seq[2], 1);
                    (void)nr;
                    ret = "PgUp";
                } else if (seq[1] == '6') {
                    nr = read(STDIN_FILENO, &seq[2], 1);
                    (void)nr;
                    ret = "PgDown";
                } else if (seq[1] == 'H') {
                    ret = "Home";
                } else if (seq[1] == 'F') {
                    ret = "End";
                }
            }
        }
    } else if (c == '\r' || c == '\n') {
        ret = "Enter";
    } else if (c == 127 || c == 8) {
        ret = "Backspace";
    } else if (c == 9) {
        ret = "Tab";
    } else if (c == ' ') {
        ret = "Space";
    } else if (c >= 1 && c <= 26 && c != 3 && c != 8 && c != 9 && c != 10 && c != 13) {
        ret << "Ctrl-";
        ret << char('A' + c - 1);
    } else if (c > 32 && c < 127) {
        ret << c;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Resolve filtertarget pointers
static void ResolveStyles() {
    acr_nav::_db.p_default_filtertarget = acr_nav::ind_filtertarget_Find("ctype");
    vrfy(acr_nav::_db.p_default_filtertarget, "filtertarget 'ctype' not found");
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
}

// -----------------------------------------------------------------------------

static void InitPanels() {
    acr_nav::_db.p_left_panel = acr_nav::ind_panel_Find("ctype_list");
    acr_nav::_db.p_right_panel = acr_nav::ind_panel_Find("content");
    vrfy(acr_nav::_db.p_left_panel, "panel 'ctype_list' not found");
    vrfy(acr_nav::_db.p_right_panel, "panel 'content' not found");
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
    acr_nav::_db.p_filter_mode = acr_nav::ind_navmode_Find("filter");
    vrfy(acr_nav::_db.p_filter_mode, "navmode 'filter' not found");
    acr_nav::_db.p_default_viewmode = acr_nav::ind_viewmode_Find("fields");
    // Resolve navaction -> helpgroup pointers (Ptr, not Upptr: 6 navactions have empty helpgroup)
    ind_beg(acr_nav::_db_navaction_curs, na, acr_nav::_db) {
        if (ch_N(na.helpgroup) > 0) {
            na.p_helpgroup = acr_nav::ind_helpgroup_Find(na.helpgroup);
        }
    } ind_end;
    // Start in help mode so new users see keybindings
    acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_default_viewmode;
    PushOverlay(acr_nav::ind_viewmode_Find("help"));
    acr_nav::_db.startup_help = true;
    ResolveStyles();
    vrfy(acr_nav::ind_navstyle_Find("title_focus"),   "navstyle 'title_focus' not found");
    vrfy(acr_nav::ind_navstyle_Find("title_nofocus"), "navstyle 'title_nofocus' not found");
    vrfy(acr_nav::ind_navstyle_Find("sel_focus"),     "navstyle 'sel_focus' not found");
    vrfy(acr_nav::ind_navstyle_Find("sel_nofocus"),   "navstyle 'sel_nofocus' not found");
    vrfy(acr_nav::ind_navstyle_Find("statusbar"),     "navstyle 'statusbar' not found");
    vrfy(acr_nav::ind_navstyle_Find("filter_match"),  "navstyle 'filter_match' not found");
    BuildHelpLines();
    SwitchToBrowse();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
    acr_nav::_db.p_right_panel->sel_row = 0;
    acr_nav::_db.p_right_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

struct PreActionState {
    acr_nav::FCtype *prev_sel_ct;
    acr_nav::FViewmode *prev_viewmode;
    int prev_depth;
    int prev_overlay_depth;
};

static PreActionState SnapshotPreAction() {
    PreActionState s;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    s.prev_sel_ct = SelectedCtype(*left);
    s.prev_viewmode = acr_nav::_db.p_cur_viewmode;
    s.prev_depth = acr_nav::navstack_N();
    s.prev_overlay_depth = acr_nav::overlay_stack_N();
    return s;
}

// -----------------------------------------------------------------------------

static void PostAction(PreActionState const &s) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    bool nsdep_ns_changed = PopOverlayOnCtypeChange(s.prev_sel_ct, sel_ct);
    bool forward = (acr_nav::navstack_N() >= s.prev_depth);
    bool vm_changed = (acr_nav::_db.p_cur_viewmode != s.prev_viewmode);
    bool ct_changed = (sel_ct != s.prev_sel_ct) || nsdep_ns_changed;
    if (ct_changed && !sel_ct) {
        ClearContentCaches();
    }
    bool overlay_pop = vm_changed && (acr_nav::overlay_stack_N() < s.prev_overlay_depth);
    if (forward && ((vm_changed && !overlay_pop) || ct_changed)) {
        right->sel_row = 0;
        right->scroll_offset = 0;
    }
    if (forward && ct_changed
        && acr_nav::_db.p_cur_viewmode == acr_nav::ind_viewmode_Find("graph")
        && sel_ct && s.prev_sel_ct && sel_ct != s.prev_sel_ct) {
        int line = GraphFindCtypeLine(*sel_ct, s.prev_sel_ct);
        if (line >= 0) {
            right->sel_row = line;
        }
    }
    AdjustScroll(*left, acr_nav::left_item_N());
    AdjustScroll(*right, RightPanelItemCount(sel_ct));
}

// -----------------------------------------------------------------------------

static bool ProcessKey(algo::strptr key_name) {
    bool did_something = false;
    {
        PreActionState snap = SnapshotPreAction();
        tempstr composite;
        composite << acr_nav::_db.p_cur_mode->navmode << "." << key_name;
        acr_nav::FKeybind *keybind = acr_nav::ind_keybind_Find(composite);
        if (keybind) {
            acr_nav::FNavaction &na = *keybind->p_navaction;
            bool is_overlay = acr_nav::_db.p_cur_viewmode->is_overlay;
            bool blocked = na.need_no_overlay && is_overlay;
            if (blocked && acr_nav::_db.startup_help) {
                acr_nav::_db.startup_help = false;
                PopOverlay();
                blocked = false;
            }
            if (blocked && ch_N(na.dismiss_viewmode) > 0
                && na.dismiss_viewmode == acr_nav::_db.p_cur_viewmode->viewmode) {
                blocked = false;
            }
            if (!blocked) {
                if (ch_N(na.target_viewmode) > 0) {
                    ToggleViewmode(acr_nav::ind_viewmode_Find(na.target_viewmode));
                } else {
                    step_Call(na);
                }
                did_something = true;
            }
        }
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (!keybind && in_filter && elems_N(key_name) == 1 && key_name[0] > 32) {
            acr_nav::_db.filter << key_name;
            BuildLeftItemsReset();
            did_something = true;
        }
        if (did_something) {
            DismissStartupHelp(keybind);
            PostAction(snap);
        }
    }
    return did_something;
}

// -----------------------------------------------------------------------------

static acr_nav::Screen BuildScreenRecord(acr_nav::FCtype *sel_ct) {
    acr_nav::Screen screen;
    screen.mode = acr_nav::_db.p_cur_mode->navmode;
    screen.focus = acr_nav::_db.p_cur_panel->panel;
    screen.filter = acr_nav::_db.filter;
    screen.navstack_depth = acr_nav::navstack_N();
    screen.n_sel_ctype = acr_nav::_db.n_visible_ctype;
    screen.n_ctype = acr_nav::ctype_N();
    screen.n_field = acr_nav::field_N();
    screen.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    screen.sel_nav_col = acr_nav::_db.sel_nav_col;
    screen.breadcrumb = BuildBreadcrumb(sel_ct);
    screen.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
    BuildStatusHint(screen.hints);
    return screen;
}

// -----------------------------------------------------------------------------

static acr_nav::PanelState BuildLeftPanelState(acr_nav::FPanel &left) {
    acr_nav::PanelState state;
    state.panel = left.panel;
    state.sel_row = left.sel_row;
    state.scroll_offset = left.scroll_offset;
    state.n_items = acr_nav::left_item_N();
    state.sel_value = "";
    if (left.sel_row >= 0 && left.sel_row < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(left.sel_row);
        if (ch_N(item.ctype) > 0) {
            state.sel_value = item.ctype;
        } else {
            state.sel_value = item.ns;
        }
    }
    return state;
}

// -----------------------------------------------------------------------------

static acr_nav::PanelState BuildRightPanelState(acr_nav::FPanel &right, acr_nav::FCtype *sel_ct) {
    acr_nav::PanelState state;
    state.panel = right.panel;
    state.sel_row = right.sel_row;
    state.scroll_offset = right.scroll_offset;
    state.n_items = RightPanelItemCount(sel_ct);
    state.sel_value = "";
    if (right.sel_row < RightPanelItemCount(sel_ct)) {
        if (!acr_nav::_db.p_cur_viewmode->has_fields) {
            state.sel_value = RightPanelLineFind(right.sel_row);
        } else if (sel_ct) {
            acr_nav::FField *fld = RightPanelFieldFind(sel_ct, right.sel_row);
            if (fld) {
                state.sel_value = fld->field;
            }
        }
    }
    return state;
}

// -----------------------------------------------------------------------------

// Emit left panel items clipped to viewport
static void EmitVisibleLeftItems(acr_nav::FPanel &left) {
    int first = left.scroll_offset;
    int last = i32_Min(first + DataRows(), acr_nav::left_item_N());
    for (int i = first; i < last; i++) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(i);
        acr_nav::VisibleLeftItem vli;
        vli.row = i;
        if (ch_N(item.ctype) > 0) {
            vli.value = item.ctype;
            vli.kind = "ctype";
            vli.collapsed = false;
            vli.n_match = 0;
            acr_nav::FCtype *ct = acr_nav::ind_ctype_Find(item.ctype);
            vli.n_record = (ct && ct->c_ssimfile) ? ct->c_ssimfile->n_record : 0;
        } else {
            vli.value = item.ns;
            vli.kind = "ns";
            acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
            vli.collapsed = ns ? ns->collapsed : false;
            vli.n_match = ns ? ns->n_match : 0;
            vli.n_record = 0;
        }
        prlog(vli);
    }
}

// -----------------------------------------------------------------------------

// Emit visible field records for field-based viewmodes (fields, xref)
static void EmitVisibleFields(acr_nav::FPanel &right, acr_nav::FCtype &sel_ct) {
    bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
    int n_vis = RightPanelItemCount(&sel_ct);
    int first = right.scroll_offset;
    int last = i32_Min(first + DataRows(), n_vis);
    for (int i = first; i < last; i++) {
        acr_nav::FField *field = RightPanelFieldFind(&sel_ct, i);
        if (field) {
            acr_nav::VisibleField vf;
            vf.row = i;
            vf.field = field->field;
            vf.arg = reverse ? field->p_ctype->ctype : field->p_arg->ctype;
            vf.reftype = field->p_reftype->reftype;
            if (field->p_reftype->c_reftypestyle) {
                vf.style = field->p_reftype->c_reftypestyle->p_navstyle->navstyle;
            }
            bool navigable = reverse
                ? (field->p_ctype != &sel_ct)
                : (field->p_arg != &sel_ct);
            vf.navigable = navigable;
            bool field_match = false;
            if (!reverse
                && acr_nav::_db.p_cur_filtertarget->has_field_criteria
                && ch_N(acr_nav::_db.filter) > 0) {
                field_match = FieldMatchesFilter(*field, acr_nav::_db.filter_regx, *acr_nav::_db.p_cur_filtertarget);
            }
            vf.match = field_match;
            prlog(vf);
        }
    }
}

// -----------------------------------------------------------------------------

// Emit visible lines for text-based viewmodes (help, preview, detail, codegen, nsdep)
static void EmitVisibleLines(acr_nav::FPanel &right, acr_nav::FCtype *sel_ct) {
    int n_lines = RightPanelItemCount(sel_ct);
    int first = right.scroll_offset;
    int last = i32_Min(first + DataRows(), n_lines);
    for (int i = first; i < last; i++) {
        acr_nav::VisibleLine vl;
        vl.row = i;
        vl.value = RightPanelLineFind(i);
        prlog(vl);
    }
    // Emit navigable column metadata for viewmodes with nav columns
    int n_nav = acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode);
    for (int i = 0; i < n_nav; i++) {
        prlog(acr_nav::nav_col_qFind(*acr_nav::_db.p_cur_viewmode, i));
    }
}

// -----------------------------------------------------------------------------

static void HeadlessOutput() {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    prlog(BuildScreenRecord(sel_ct));
    prlog(BuildLeftPanelState(*left));
    EmitVisibleLeftItems(*left);
    prlog(BuildRightPanelState(*right, sel_ct));
    if (sel_ct && acr_nav::_db.p_cur_viewmode->has_fields) {
        EmitVisibleFields(*right, *sel_ct);
    } else {
        EmitVisibleLines(*right, sel_ct);
    }
    prlog("");
}

// -----------------------------------------------------------------------------

static void EmitAck(algo::strptr cmd_tag, bool ok, algo::strptr msg) {
    acr_nav::Ack ack;
    ack.ack = cmd_tag;
    ack.ok = ok;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    if (sel_ct) {
        ack.ctype = sel_ct->ctype;
    }
    ack.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    ack.navstack_depth = acr_nav::navstack_N();
    ack.msg << msg;
    prlog(ack);
    prlog("");
}

// -----------------------------------------------------------------------------

static void EmitSummary() {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FPanel *right = acr_nav::_db.p_right_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    prlog(BuildScreenRecord(sel_ct));
    prlog(BuildLeftPanelState(*left));
    prlog(BuildRightPanelState(*right, sel_ct));
    prlog("");
}

// -----------------------------------------------------------------------------

static void HeadlessGoBack() {
    acr_nav::_db.startup_help = false;
    bool ok = !acr_nav::navstack_EmptyQ();
    if (ok) {
        PreActionState snap = SnapshotPreAction();
        acr_nav::navaction_go_back();
        PostAction(snap);
    }
    EmitAck("acr_nav.GoBack", ok, ok ? "" : "navstack is empty");
}

// -----------------------------------------------------------------------------

static void HeadlessSetView(acr_nav::SetView &cmd) {
    acr_nav::_db.startup_help = false;
    acr_nav::FViewmode *vm = acr_nav::ind_viewmode_Find(cmd.viewmode);
    if (!vm) {
        EmitAck("acr_nav.SetView", false, tempstr() << "viewmode not found: " << cmd.viewmode);
    } else if (vm->scope_ns) {
        EmitAck("acr_nav.SetView", false, "namespace-scoped viewmode is context-sensitive");
    } else if (vm == acr_nav::ind_viewmode_Find("detail")) {
        EmitAck("acr_nav.SetView", false, "use SendKey key:d on a field");
    } else {
        // Cancel filter mode if active
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (in_filter) {
            acr_nav::navaction_filter_cancel();
        }
        PreActionState snap = SnapshotPreAction();
        if (vm->is_overlay) {
            PushOverlay(vm);
        } else {
            // Pop any active overlays first
            while (!acr_nav::overlay_stack_EmptyQ()) {
                PopOverlay();
            }
            acr_nav::_db.p_cur_viewmode = vm;
        }
        PostAction(snap);
        EmitAck("acr_nav.SetView", true, "");
    }
}

// -----------------------------------------------------------------------------

static void HeadlessSetFilter(acr_nav::SetFilter &cmd) {
    acr_nav::_db.startup_help = false;
    acr_nav::FFiltertarget *ft = acr_nav::ind_filtertarget_Find(cmd.target);
    bool ok = (ft != nullptr);
    tempstr err_msg;
    if (!ok) {
        err_msg << "invalid target: " << cmd.target;
    }
    if (ok) {
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (in_filter) {
            acr_nav::navaction_filter_cancel();
        }
        PreActionState snap = SnapshotPreAction();
        bool clearing = (elems_N(algo::Trimmed(cmd.filter)) == 0);
        if (clearing) {
            // Empty filter clears the active filter without pushing navstack
            acr_nav::_db.filter = "";
            acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
            BuildLeftItemsReset();
        } else {
            // Full filter workflow: start saves pre-filter state for backtrack
            acr_nav::navaction_filter_start();
            acr_nav::_db.filter = cmd.filter;
            acr_nav::_db.p_cur_filtertarget = ft;
            BuildLeftItemsReset();
            acr_nav::navaction_filter_accept();
        }
        PostAction(snap);
    }
    EmitAck("acr_nav.SetFilter", ok, err_msg);
}

// -----------------------------------------------------------------------------

static void HeadlessNavigate(acr_nav::Navigate &cmd) {
    acr_nav::_db.startup_help = false;
    bool ok = true;
    tempstr err_msg;
    acr_nav::FViewmode *dest_viewmode = acr_nav::_db.p_cur_viewmode;
    if (ch_N(cmd.ctype) == 0) {
        ok = false;
        err_msg << "ctype is empty";
    }
    acr_nav::FCtype *target = ok ? acr_nav::ind_ctype_Find(cmd.ctype) : nullptr;
    if (ok && !target) {
        ok = false;
        err_msg << "ctype not found: " << cmd.ctype;
    }
    // If overlay is current, use fields as default destination
    if (ok && acr_nav::_db.p_cur_viewmode->is_overlay) {
        dest_viewmode = acr_nav::_db.p_default_viewmode;
    }
    // Validate viewmode if specified
    if (ok && ch_N(cmd.viewmode) > 0) {
        acr_nav::FViewmode *vm = acr_nav::ind_viewmode_Find(cmd.viewmode);
        if (!vm) {
            ok = false;
            err_msg << "viewmode not found: " << cmd.viewmode;
        } else if (vm->scope_ns) {
            ok = false;
            err_msg << "namespace-scoped viewmode is context-sensitive";
        } else if (vm->is_overlay) {
            ok = false;
            err_msg << "overlay viewmodes not supported in Navigate";
        } else if (vm->need_ssimfile && !FindSsimfile(*target)) {
            ok = false;
            err_msg << "viewmode " << cmd.viewmode << " requires ssimfile for " << cmd.ctype;
        } else {
            dest_viewmode = vm;
        }
    }
    if (ok) {
        bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
        if (in_filter) {
            acr_nav::navaction_filter_cancel();
        }
        // Pop overlays before navigation so GoToCtype's viewmode sticks
        while (!acr_nav::overlay_stack_EmptyQ()) {
            PopOverlay();
        }
        PreActionState snap = SnapshotPreAction();
        acr_nav::GoToCtype(cmd.ctype, dest_viewmode);
        PostAction(snap);
    }
    EmitAck("acr_nav.Navigate", ok, err_msg);
}

// -----------------------------------------------------------------------------

// Headless dispatch handler: send a key
void acr_nav::Headless_SendKey(acr_nav::SendKey& msg) {
    ProcessKey(msg.key);
}

// Headless dispatch handler: take a screenshot
void acr_nav::Headless_Screenshot(acr_nav::Screenshot&) {
    HeadlessOutput();
}

// Headless dispatch handler: resize terminal
void acr_nav::Headless_SetTermSize(acr_nav::SetTermSize& msg) {
    acr_nav::_db.term_hei = i32_Max(1, msg.term_hei);
    acr_nav::_db.term_wid = i32_Max(1, msg.term_wid);
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    AdjustScroll(*left, acr_nav::left_item_N());
    AdjustScroll(*acr_nav::_db.p_right_panel, RightPanelItemCount(sel_ct));
}

// Headless dispatch handler: navigate to a ctype
void acr_nav::Headless_Navigate(acr_nav::Navigate& msg) {
    HeadlessNavigate(msg);
}

// Headless dispatch handler: set filter
void acr_nav::Headless_SetFilter(acr_nav::SetFilter& msg) {
    HeadlessSetFilter(msg);
}

// Headless dispatch handler: set view mode
void acr_nav::Headless_SetView(acr_nav::SetView& msg) {
    HeadlessSetView(msg);
}

// Headless dispatch handler: go back
void acr_nav::Headless_GoBack(acr_nav::GoBack&) {
    HeadlessGoBack();
}

// Headless dispatch handler: emit summary
void acr_nav::Headless_Summary(acr_nav::Summary&) {
    EmitSummary();
}

// Headless dispatch handler: request state dump
void acr_nav::Headless_RequestStateDump(acr_nav::RequestStateDump& msg) {
    algo_lib::Regx filter;
    Regx_ReadSql(filter, msg.filter, true);
    algo::cstring out;
    acr_nav::StateDump(out, filter);
    prlog(out);
}

// Headless dispatch handler: unrecognized text input
void acr_nav::Headless_UnkText(algo::strptr line) {
    acr_nav::InputError err;
    err.lineno = acr_nav::_db.headless_lineno;
    err.msg << "unrecognized input: " << line;
    prlog(err);
}

// Parse and dispatch one headless protocol command
static void DispatchHeadlessCommand(algo::strptr line, int lineno) {
    if (elems_N(algo::Trimmed(line)) == 0) {
        // empty lines are ssim separators, not errors
    } else {
        acr_nav::_db.headless_lineno = lineno;
        acr_nav::Headless_DispatchText(line);
    }
}

// -----------------------------------------------------------------------------

static algo::LineBuf _stdin_linebuf;
static int _stdin_lineno;

// Epoll callback for stdin in headless+ipc mode.
// Edge-triggered: must drain the fd completely on each invocation.
static void StdinReadCallback() {
    bool done = false;
    bool shutdown = false;
    while (!done) {
        char buf[4096];
        ssize_t nr = read(STDIN_FILENO, buf, sizeof(buf));
        if (nr > 0) {
            algo::LinebufBegin(_stdin_linebuf, algo::memptr((u8*)buf, nr), false);
            algo::strptr line;
            while (acr_nav::_db.running && algo::LinebufNext(_stdin_linebuf, line)) {
                ++_stdin_lineno;
                DispatchHeadlessCommand(line, _stdin_lineno);
            }
            // Quit command sets _db.running = false -- stop draining
            done = !acr_nav::_db.running;
            shutdown = done;
        } else if (nr == 0) {
            // EOF -- flush partial line, emit final output, request exit
            algo::LinebufBegin(_stdin_linebuf, algo::memptr(NULL, 0), true);
            algo::strptr line;
            while (algo::LinebufNext(_stdin_linebuf, line)) {
                ++_stdin_lineno;
                DispatchHeadlessCommand(line, _stdin_lineno);
            }
            HeadlessOutput();
            algo_lib::ReqExitMainLoop();
            done = true;
        } else {
            // nr < 0
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                done = true; // no more data right now
            } else {
                HeadlessOutput();
                algo_lib::ReqExitMainLoop();
                done = true;
            }
        }
    }
    if (shutdown) {
        HeadlessOutput();
        algo_lib::ReqExitMainLoop();
    }
}

// Set up stdin as a non-blocking iohook for headless+ipc combined mode
static void HeadlessIpcInit() {
    InitPanels();
    acr_nav::_db.running = true;
    algo::SetBlockingMode(algo::Fildes(STDIN_FILENO), false);
    acr_nav::_db.stdin_iohook.fildes = algo::Fildes(STDIN_FILENO);
    acr_nav::_db.stdin_iohook.nodelete = true;
    callback_Set0(acr_nav::_db.stdin_iohook, StdinReadCallback);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(acr_nav::_db.stdin_iohook, flags);
}

// -----------------------------------------------------------------------------

// Shared repaint logic for TUI+IPC mode
static void TuiRepaint() {
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    DetectTerminal();
    cstring render_buf;
    Render(render_buf, sel_ct);
    WriteStdout(render_buf.ch_elems, ch_N(render_buf));
}

// -----------------------------------------------------------------------------

// Bare-ESC timeout: 50ms passed after ESC with no follow-up byte
static void EscTimeoutCallback() {
    acr_nav_esc_timer_armed = false;
    acr_nav_keybuf_n = 0;
    bool repaint = ProcessKey("Escape");
    if (repaint) {
        TuiRepaint();
    }
}

// -----------------------------------------------------------------------------

// Recurrent 50ms check for SIGWINCH flag
static void SigwinchCheckCallback() {
    if (acr_nav_sigwinch) {
        acr_nav_sigwinch = 0;
        TuiRepaint();
    }
}

// -----------------------------------------------------------------------------

// Non-blocking byte-at-a-time VT100 key decoder for TUI+IPC mode.
// Returns non-empty key name when a complete key is recognized.
// Returns empty string when more bytes are needed.
static tempstr DecodeKeyByte(char c) {
    tempstr ret;
    if (acr_nav_keybuf_n == 0) {
        if (c == 3) {
            acr_nav::_db.running = false;
        } else if (c == '\x1b') {
            acr_nav_keybuf[0] = c;
            acr_nav_keybuf_n = 1;
            acr_nav_esc_timer_armed = true;
            algo_lib::ThScheduleIn(acr_nav_esc_timer, algo::ToSchedTime(0.050));
        } else if (c == '\r' || c == '\n') {
            ret = "Enter";
        } else if (c == 127 || c == 8) {
            ret = "Backspace";
        } else if (c == 9) {
            ret = "Tab";
        } else if (c == ' ') {
            ret = "Space";
        } else if (c >= 1 && c <= 26 && c != 3 && c != 8 && c != 9 && c != 10 && c != 13) {
            ret << "Ctrl-";
            ret << char('A' + c - 1);
        } else if (c > 32 && c < 127) {
            ret << c;
        }
    } else if (acr_nav_keybuf_n == 1 && acr_nav_keybuf[0] == '\x1b') {
        if (acr_nav_esc_timer_armed) {
            algo_lib::bh_timehook_Remove(acr_nav_esc_timer);
            acr_nav_esc_timer_armed = false;
        }
        if (c == '[') {
            acr_nav_keybuf[1] = c;
            acr_nav_keybuf_n = 2;
        } else {
            acr_nav_keybuf_n = 0;
            ret = "Escape";
        }
    } else if (acr_nav_keybuf_n == 2) {
        if (c == 'A') {
            ret = "Up";
        } else if (c == 'B') {
            ret = "Down";
        } else if (c == 'C') {
            ret = "Right";
        } else if (c == 'D') {
            ret = "Left";
        } else if (c == 'H') {
            ret = "Home";
        } else if (c == 'F') {
            ret = "End";
        } else if (c == '5' || c == '6') {
            acr_nav_keybuf[2] = c;
            acr_nav_keybuf_n = 3;
        } else {
            acr_nav_keybuf_n = 0;
        }
        if (ch_N(ret) > 0) {
            acr_nav_keybuf_n = 0;
        }
    } else if (acr_nav_keybuf_n == 3) {
        if (c == '~') {
            if (acr_nav_keybuf[2] == '5') {
                ret = "PgUp";
            } else {
                ret = "PgDown";
            }
        }
        acr_nav_keybuf_n = 0;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Epoll callback for stdin in TUI+IPC mode.
// Edge-triggered: must drain the fd completely on each invocation.
// Batches repaints: one render per drain cycle regardless of key count.
static void TuiStdinReadCallback() {
    bool done = false;
    bool any_repaint = false;
    while (!done && acr_nav::_db.running) {
        char buf[256];
        ssize_t nr = read(STDIN_FILENO, buf, sizeof(buf));
        if (nr > 0) {
            for (ssize_t i = 0; i < nr && acr_nav::_db.running; i++) {
                tempstr key_name = DecodeKeyByte(buf[i]);
                if (ch_N(key_name) > 0) {
                    any_repaint = ProcessKey(key_name) || any_repaint;
                }
            }
        } else if (nr == 0) {
            acr_nav::_db.running = false;
            done = true;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                done = true;
            } else {
                acr_nav::_db.running = false;
                done = true;
            }
        }
    }
    if (any_repaint && acr_nav::_db.running) {
        TuiRepaint();
    }
    if (!acr_nav::_db.running) {
        algo_lib::ReqExitMainLoop();
    }
}

// -----------------------------------------------------------------------------

// Set up TUI with non-blocking stdin + IPC socket, both under epoll
static void TuiIpcInit() {
    InitPanels();
    DetectTerminal();
    EnterRawMode();
    struct sigaction sa;
    sa.sa_handler = SigwinchHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGWINCH, &sa, NULL);
    algo::SetBlockingMode(algo::Fildes(STDIN_FILENO), false);
    acr_nav::_db.stdin_iohook.fildes = algo::Fildes(STDIN_FILENO);
    acr_nav::_db.stdin_iohook.nodelete = true;
    callback_Set0(acr_nav::_db.stdin_iohook, TuiStdinReadCallback);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(acr_nav::_db.stdin_iohook, flags);
    hook_Set0(acr_nav_esc_timer, EscTimeoutCallback);
    hook_Set0(acr_nav_sigwinch_timer, SigwinchCheckCallback);
    algo_lib::ThInitRecur(acr_nav_sigwinch_timer, algo::ToSchedTime(0.050));
    algo_lib::bh_timehook_Insert(acr_nav_sigwinch_timer);
    acr_nav::_db.running = true;
    TuiRepaint();
}

// -----------------------------------------------------------------------------

static void HeadlessMain() {
    InitPanels();
    acr_nav::_db.running = true;
    algo::LineBuf linebuf;
    bool eof = false;
    int lineno = 0;
    while (acr_nav::_db.running && !eof) {
        char buf[4096];
        ssize_t nr = read(STDIN_FILENO, buf, sizeof(buf));
        eof = (nr <= 0);
        algo::LinebufBegin(linebuf, algo::memptr((u8*)buf, eof ? 0 : nr), eof);
        algo::strptr line;
        while (acr_nav::_db.running && algo::LinebufNext(linebuf, line)) {
            ++lineno;
            DispatchHeadlessCommand(line, lineno);
        }
    }
    HeadlessOutput();
}

// -----------------------------------------------------------------------------

static void CountSsimfileRecords() {
    ind_beg(acr_nav::_db_ssimfile_curs, ssimfile, acr_nav::_db) {
        tempstr fname(SsimFname(acr_nav::_db.cmdline.in, ssimfile.ssimfile));
        ind_beg(algo::FileLine_curs, line, fname) {
            (void)line;
            ssimfile.n_record++;
        } ind_end;
    } ind_end;
}

// -----------------------------------------------------------------------------

void acr_nav::Main() {
    CountSsimfileRecords();
    BuildLeftItems();
    if (_db.cmdline.ipc) {
        acr_nav::IpcInit();
    }
    bool do_dump = ch_N(_db.cmdline.dump) > 0;
    bool headless = _db.cmdline.headless || !isatty(STDOUT_FILENO);
    if (do_dump) {
        algo_lib::Regx filter;
        Regx_ReadSql(filter, _db.cmdline.dump, true);
        algo::cstring out;
        StateDump(out, filter);
        prlog(out);
    } else if (_db.cmdline.ipc && headless) {
        HeadlessIpcInit();
        acr_nav::MainLoop();
    } else if (_db.cmdline.ipc) {
        TuiIpcInit();
        acr_nav::MainLoop();
        algo_lib::bh_timehook_Remove(acr_nav_esc_timer);
        algo_lib::bh_timehook_Remove(acr_nav_sigwinch_timer);
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGWINCH, &sa, NULL);
        ExitRawMode();
    } else if (headless) {
        HeadlessMain();
    } else {
        InitPanels();
        DetectTerminal();
        EnterRawMode();
        struct sigaction sa;
        sa.sa_handler = SigwinchHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0; // no SA_RESTART — read() must return EINTR
        sigaction(SIGWINCH, &sa, NULL);
        acr_nav::FCtype *sel_ct = SelectedCtype(*_db.p_left_panel);
        DetectTerminal();
        {
            cstring render_buf;
            Render(render_buf, sel_ct);
            WriteStdout(render_buf.ch_elems, ch_N(render_buf));
        }
        _db.running = true;
        while (_db.running) {
            tempstr key_name = ReadKeyName();
            bool repaint = acr_nav_sigwinch;
            acr_nav_sigwinch = 0;
            repaint = ProcessKey(key_name) || repaint;
            if (repaint) {
                sel_ct = SelectedCtype(*_db.p_left_panel);
                DetectTerminal();
                cstring render_buf;
                Render(render_buf, sel_ct);
                WriteStdout(render_buf.ch_elems, ch_N(render_buf));
            }
        }
        sa.sa_handler = SIG_DFL;
        sigaction(SIGWINCH, &sa, NULL);
        ExitRawMode();
    }
}
