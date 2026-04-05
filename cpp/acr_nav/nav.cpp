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
// Source: cpp/acr_nav/nav.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

using acr_nav::CtypeMatchesFilter;
using acr_nav::SelectedCtype;
using acr_nav::SelectedNs;
using acr_nav::PanelItemCount;
using acr_nav::RightPanelItemCount;
using acr_nav::SwitchToBrowse;
using acr_nav::IsHelpMode;
using acr_nav::IsDetailMode;
using acr_nav::IsNsDepMode;
using acr_nav::NsDepNsAtLine;
using acr_nav::NsDisplayName;
using acr_nav::RightPanelLineFind;
using acr_nav::RightPanelFieldFind;
using acr_nav::DisplayToByte;
using acr_nav::DataRows;
using acr_nav::FindSsimfile;
using acr_nav::LoadNsDep;
using acr_nav::LoadDetail;
using acr_nav::BuildLeftItems;
using acr_nav::PopOverlay;
using acr_nav::PushOverlay;
using acr_nav::PopOverlayOnCtypeChange;
using acr_nav::DismissStartupHelp;
using acr_nav::BuildLeftItemsReset;

// Scan left_item array for a ctype match. Returns index or -1.
static int FindLeftItemByCtype(algo::strptr ctype_key) {
    int result = -1;
    for (int i = 0; i < acr_nav::left_item_N(); i++) {
        if (acr_nav::left_item_qFind(i).ctype == ctype_key) {
            result = i;
            break;
        }
    }
    return result;
}

// Scan left_item for namespace header match (ns == key AND ctype is empty). Returns index or -1.
static int FindLeftItemByNsHeader(algo::strptr ns_key) {
    int result = -1;
    for (int i = 0; i < acr_nav::left_item_N(); i++) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(i);
        if (item.ns == ns_key && ch_N(item.ctype) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

// -----------------------------------------------------------------------------

// Collect namespaces that have at least one ctype matching the filter.
static void CollectMatchingNamespaces(acr_nav::FNs **ns_arr, int &n_ns, algo_lib::Regx &filter_regx, bool has_filter, acr_nav::FFiltertarget &ft) {
    n_ns = 0;
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        int n_match = 0;
        for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
            acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
            if (ct && ch_N(ct->ctype) > 0) {
                bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, ft);
                n_match += match;
            }
        }
        ns.n_match = n_match;
        if (n_match > 0 && n_ns < 256) {
            ns_arr[n_ns++] = &ns;
        }
    } ind_end;
}

// Insertion sort namespace array by name.
static void InsertionSortNsByName(acr_nav::FNs **ns_arr, int n_ns) {
    for (int i = 1; i < n_ns; i++) {
        acr_nav::FNs *tmp = ns_arr[i];
        int j = i;
        while (j > 0 && algo::strptr_Cmp(ns_arr[j - 1]->ns, tmp->ns) > 0) {
            ns_arr[j] = ns_arr[j - 1];
            j--;
        }
        ns_arr[j] = tmp;
    }
}

// -----------------------------------------------------------------------------

void acr_nav::BuildLeftItems() {
    acr_nav::left_item_RemoveAll();
    acr_nav::_db.n_visible_ctype = 0;
    algo_lib::Regx filter_regx;
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        tempstr pattern;
        pattern << "%" << acr_nav::_db.filter << "%";
        algo::MakeLower(pattern);
        algo_lib::Regx_ReadSql(filter_regx, pattern, false);
    }
    acr_nav::_db.filter_regx = filter_regx;
    acr_nav::FFiltertarget &ft = *acr_nav::_db.p_cur_filtertarget;
    // Collect namespaces with matching ctypes, sorted alphabetically.
    // FNs records are loaded from data/ in file order; explicit sort guarantees
    // stable display regardless of load order.
    acr_nav::FNs *ns_arr[256]; // fixed capacity; silently truncates if exceeded
    int n_ns = 0;
    CollectMatchingNamespaces(ns_arr, n_ns, filter_regx, has_filter, ft);
    InsertionSortNsByName(ns_arr, n_ns);
    // Build display list
    for (int ni = 0; ni < n_ns; ni++) {
        acr_nav::FNs &ns = *ns_arr[ni];
        // Namespace header
        acr_nav::LeftItem &hdr = acr_nav::left_item_Alloc();
        hdr.ctype = "";
        hdr.ns = ns.ns;
        acr_nav::_db.n_visible_ctype += ns.n_match;
        // Ctype rows (if expanded)
        if (!ns.collapsed) {
            for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
                acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
                if (ct && ch_N(ct->ctype) > 0) {
                    bool match = !has_filter || CtypeMatchesFilter(*ct, filter_regx, ft);
                    if (match) {
                        acr_nav::LeftItem &item = acr_nav::left_item_Alloc();
                        item.ctype = ct->ctype;
                        item.ns = "";
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void acr_nav::BuildLeftItemsReset() {
    BuildLeftItems();
    acr_nav::_db.p_left_panel->sel_row = 0;
    acr_nav::_db.p_left_panel->scroll_offset = 0;
}

// -----------------------------------------------------------------------------

// Build left panel from PoolCensus lines in live_data.
// Called on each complete poll response in connect mode.
void acr_nav::BuildLiveLeftItems() {
    // Save current selection for restoration after rebuild
    tempstr saved_ctype;
    int saved_row = acr_nav::_db.p_left_panel->sel_row;
    if (saved_row >= 0 && saved_row < acr_nav::left_item_N()) {
        saved_ctype = acr_nav::left_item_qFind(saved_row).ctype;
    }
    acr_nav::left_item_RemoveAll();
    // Parse PoolCensus lines into pool_entry Tary
    acr_nav::pool_entry_RemoveAll();
    ind_beg(Line_curs, line, acr_nav::_db.live_data) {
        if (algo::StartsWithQ(line, strptr("report.PoolCensus"))) {
            algo::Tuple tuple;
            if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                algo::strptr ct = algo::attr_GetString(tuple, "ctype");
                algo::strptr nr = algo::attr_GetString(tuple, "n_record");
                if (elems_N(ct) > 0) {
                    acr_nav::PoolEntry &pe = acr_nav::pool_entry_Alloc();
                    pe.ctype = ct;
                    pe.n_record = algo::ParseI32(nr, 0);
                }
            }
        }
    } ind_end;
    // Sort alphabetically by ctype name
    int n = acr_nav::pool_entry_N();
    for (int i = 1; i < n; i++) {
        for (int j = i; j > 0 && acr_nav::pool_entry_qFind(j - 1).ctype > acr_nav::pool_entry_qFind(j).ctype; j--) {
            acr_nav::PoolEntry tmp = acr_nav::pool_entry_qFind(j);
            acr_nav::pool_entry_qFind(j) = acr_nav::pool_entry_qFind(j - 1);
            acr_nav::pool_entry_qFind(j - 1) = tmp;
        }
    }
    // Namespace header
    acr_nav::LeftItem &hdr = acr_nav::left_item_Alloc();
    hdr.ctype = "";
    hdr.ns = acr_nav::_db.live_ns;
    hdr.n_record = n;
    // Pool rows
    for (int i = 0; i < n; i++) {
        acr_nav::PoolEntry &pe = acr_nav::pool_entry_qFind(i);
        acr_nav::LeftItem &item = acr_nav::left_item_Alloc();
        item.ctype = pe.ctype;
        item.ns = "";
        item.n_record = pe.n_record;
    }
    acr_nav::_db.n_visible_ctype = n;
    // Restore selection
    int idx = FindLeftItemByCtype(saved_ctype);
    if (idx >= 0) {
        acr_nav::_db.p_left_panel->sel_row = idx;
    } else if (saved_row >= acr_nav::left_item_N()) {
        acr_nav::_db.p_left_panel->sel_row = i32_Max(0, acr_nav::left_item_N() - 1);
    }
}

// -----------------------------------------------------------------------------

static void RecollapseAutoExpanded() {
    ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
        if (ns.auto_expanded) {
            ns.collapsed = true;
            ns.auto_expanded = false;
        }
    } ind_end;
}

// -----------------------------------------------------------------------------

// Used by show_help toggle, show_detail toggle, and startup help dismiss.
void acr_nav::PopOverlay() {
    if (!acr_nav::overlay_stack_EmptyQ()) {
        acr_nav::OverlayEntry &entry = acr_nav::overlay_stack_qLast();
        acr_nav::_db.p_cur_viewmode = acr_nav::ind_viewmode_Find(entry.viewmode);
        acr_nav::_db.p_right_panel->sel_row = entry.saved_sel_row;
        acr_nav::_db.p_right_panel->scroll_offset = entry.saved_scroll_offset;
        acr_nav::overlay_stack_RemoveLast();
    }
}

// Push current viewmode + right-panel state onto overlay stack, switch to target.
void acr_nav::PushOverlay(acr_nav::FViewmode *target) {
    acr_nav::OverlayEntry &entry = acr_nav::overlay_stack_Alloc();
    entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    entry.saved_sel_row = acr_nav::_db.p_right_panel->sel_row;
    entry.saved_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
    acr_nav::_db.p_cur_viewmode = target;
}

// Dismiss startup help on any non-passive action (not movement/panel switch).
// Escape/? pop the overlay directly; detected here via !IsHelpMode().
void acr_nav::DismissStartupHelp(acr_nav::FKeybind *keybind) {
    if (acr_nav::_db.startup_help) {
        bool is_action = keybind && !keybind->p_navaction->passive;
        if (is_action || !IsHelpMode()) {
            acr_nav::_db.startup_help = false;
            if (IsHelpMode()) {
                PopOverlay();
            }
        }
    }
}

// Restore the viewmode saved before entering nsdep mode, or fall back to default.
static void DeactivateNsdep() {
    acr_nav::_db.p_cur_viewmode = acr_nav::_db.p_pre_nsdep_viewmode
        ? acr_nav::_db.p_pre_nsdep_viewmode : acr_nav::_db.p_default_viewmode;
    acr_nav::_db.p_pre_nsdep_viewmode = NULL;
}

// If an overlay viewmode is active and the selected ctype changed, pop all
// overlays and restore the base viewmode.  During startup help, preserve the
// overlay so movement doesn't dismiss it.
// nsdep is a context-sensitive view: auto-activates on namespace headers,
// restores the previous viewmode when leaving.
bool acr_nav::PopOverlayOnCtypeChange(acr_nav::FCtype *prev_sel_ct, acr_nav::FCtype *sel_ct) {
    bool nsdep_changed = false;
    // nsdep context: activate on ns headers, deactivate on ctype rows, reload on ns change
    if (IsNsDepMode()) {
        if (sel_ct) {
            // Leaving ns header for ctype row — restore saved viewmode
            DeactivateNsdep();
            nsdep_changed = true;
        } else {
            acr_nav::FNs *ns = SelectedNs();
            if (ns && ns != acr_nav::_db.p_nsdep_ns) {
                acr_nav::FCtype *proxy = acr_nav::c_ctype_N(*ns) > 0
                    ? acr_nav::c_ctype_Find(*ns, 0) : nullptr;
                if (proxy) {
                    acr_nav::ensure_content_Call(*acr_nav::_db.p_cur_viewmode, *proxy);
                }
                acr_nav::_db.p_nsdep_ns = ns;
                nsdep_changed = true;
            }
        }
    }
    bool changed = sel_ct != prev_sel_ct || nsdep_changed;
    if (changed && !acr_nav::overlay_stack_EmptyQ() && !acr_nav::_db.startup_help) {
        acr_nav::FViewmode *base = acr_nav::ind_viewmode_Find(acr_nav::overlay_stack_qFind(0).viewmode);
        // RemoveAll intentionally bypasses PopOverlay — saved panel state is
        // discarded because ct_changed triggers a reset in ProcessKey.
        acr_nav::overlay_stack_RemoveAll();
        acr_nav::_db.p_cur_viewmode = base;
        acr_nav::_db.p_detail_field = NULL;
        // If overlay-pop restored nsdep as base but we landed on a ctype row,
        // deactivate nsdep immediately (same logic as the IsNsDepMode block above).
        if (IsNsDepMode() && sel_ct) {
            DeactivateNsdep();
            nsdep_changed = true;
        }
    }
    // Auto-activate scope_ns when landing on a namespace-header row
    if (!IsNsDepMode() && !sel_ct && !acr_nav::_db.p_cur_viewmode->is_overlay) {
        acr_nav::FNs *ns = SelectedNs();
        if (ns) {
            if (acr_nav::_db.startup_help) {
                acr_nav::_db.startup_help = false;
                if (IsHelpMode()) {
                    PopOverlay();
                }
            }
            acr_nav::_db.p_pre_nsdep_viewmode = acr_nav::_db.p_cur_viewmode;
            LoadNsDep(*ns);
            acr_nav::_db.p_cur_viewmode = acr_nav::ind_viewmode_Find("nsdep");
            nsdep_changed = true;
        }
    }
    return nsdep_changed;
}

// -----------------------------------------------------------------------------

// Push a snapshot of the current navigation state onto the navstack.
// entry_ctype: the ctype key (or namespace display name for nsdep) to record.
static void PushNaventry(algo::strptr entry_ctype) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::Naventry &entry = acr_nav::navstack_Alloc();
    entry.filter = acr_nav::_db.filter;
    entry.navmode = acr_nav::_db.p_cur_mode->navmode;
    entry.scroll_offset = left->scroll_offset;
    entry.sel_row = left->sel_row;
    entry.right_sel_row = acr_nav::_db.p_right_panel->sel_row;
    entry.right_scroll_offset = acr_nav::_db.p_right_panel->scroll_offset;
    entry.viewmode = acr_nav::_db.p_cur_viewmode->viewmode;
    entry.ctype = entry_ctype;
    entry.filtertarget = acr_nav::_db.p_cur_filtertarget->filtertarget;
    entry.focus_panel = acr_nav::_db.p_cur_panel->panel;
    entry.sel_nav_col = acr_nav::_db.sel_nav_col;
}

// -----------------------------------------------------------------------------

// Clear filter/browse state and select a ctype in the left panel.
// Does not modify viewmode or navstack — callers handle those.
static void RevealCtype(acr_nav::FCtype *target) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    target->p_ns->collapsed = false;
    acr_nav::_db.filter = "";
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    SwitchToBrowse();
    BuildLeftItems();
    int idx = FindLeftItemByCtype(target->ctype);
    if (idx >= 0) {
        left->sel_row = idx;
    }
}

// -----------------------------------------------------------------------------

// Push navstack, navigate to target ctype, set dest_viewmode.
// Shared by field/xref follow and graph follow.
static void NavigateToTarget(acr_nav::FCtype *sel_ct, acr_nav::FCtype *target, acr_nav::FViewmode *dest_viewmode) {
    PushNaventry(sel_ct->ctype);
    RevealCtype(target);
    acr_nav::_db.p_cur_viewmode = dest_viewmode;
}

// -----------------------------------------------------------------------------

// Navigate directly to a ctype by key. Used by headless Navigate command.
// If sel_ct is non-null (already viewing a ctype), pushes navstack via NavigateToTarget.
// If sel_ct is null (initial state on namespace header), navigates without pushing navstack.
// Returns the target FCtype, or nullptr if not found.
acr_nav::FCtype* acr_nav::GoToCtype(algo::strptr ctype_key, acr_nav::FViewmode *dest_viewmode) {
    acr_nav::FCtype *target = acr_nav::ind_ctype_Find(ctype_key);
    if (target) {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        if (sel_ct) {
            // Already viewing a ctype — push navstack and navigate
            NavigateToTarget(sel_ct, target, dest_viewmode);
        } else {
            // Initial state (cursor on namespace header) — navigate without pushing navstack
            RevealCtype(target);
            acr_nav::_db.p_cur_viewmode = dest_viewmode;
        }
    }
    return target;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_move_up() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    panel.sel_row = i32_Max(0, panel.sel_row - 1);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_move_down() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    panel.sel_row = i32_Min(last, panel.sel_row + 1);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_page_up() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    int page = DataRows();
    panel.sel_row = i32_Max(0, panel.sel_row - page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_page_down() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    int page = DataRows();
    panel.sel_row = i32_Min(last, panel.sel_row + page);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_switch_panel_left() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    if (panel.position > acr_nav::_db.p_left_panel->position) {
        int n_nav = acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode);
        if (n_nav > 0 && acr_nav::_db.sel_nav_col > 0) {
            acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col - 1;
        } else {
            acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
        }
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_switch_panel_right() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    if (panel.position < acr_nav::_db.p_right_panel->position) {
        if (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode) {
            SwitchToBrowse();
        }
        acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
    } else {
        int n_nav = acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode);
        if (n_nav > 0 && acr_nav::_db.sel_nav_col < n_nav - 1) {
            acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col + 1;
        }
    }
}

// -----------------------------------------------------------------------------

// Left panel Enter: toggle namespace collapse or switch to right panel.
static void FollowRefLeftPanel(acr_nav::FPanel *left) {
    int sel = left->sel_row;
    if (sel >= 0 && sel < acr_nav::left_item_N()) {
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(sel);
        if (ch_N(item.ctype) == 0) {
            acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
            if (ns) {
                ns->collapsed = !ns->collapsed;
                if (ns->collapsed) {
                    ns->auto_expanded = false;
                }
                BuildLeftItems();
            }
        } else {
            // Leave nsdep context so right panel returns to fields view
            if (IsNsDepMode()) {
                DeactivateNsdep();
            }
            acr_nav::_db.p_cur_panel = acr_nav::_db.p_right_panel;
        }
    }
}

// -----------------------------------------------------------------------------

// NsDep mode Enter: jump to the namespace on the selected line.
// Keeps the target namespace collapsed and selects its header row.
static void FollowRefNsDep(acr_nav::FPanel &panel, acr_nav::FPanel *left, acr_nav::FCtype *sel_ct) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_cur_viewmode;
    acr_nav::FNs *target_ns = NsDepNsAtLine(vm, panel.sel_row);
    if (target_ns) {
        PushNaventry(sel_ct ? algo::strptr(sel_ct->ctype)
            : (acr_nav::_db.p_nsdep_ns ? NsDisplayName(*acr_nav::_db.p_nsdep_ns)
                                       : algo::strptr("")));
        acr_nav::_db.filter = "";
        acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
        SwitchToBrowse();
        BuildLeftItems();
        int idx = FindLeftItemByNsHeader(target_ns->ns);
        if (idx >= 0) {
            left->sel_row = idx;
        }
    }
}

// -----------------------------------------------------------------------------

// Fields/xref mode Enter: follow the field reference (forward or reverse).
static void FollowRefFields(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    acr_nav::FField *fld = RightPanelFieldFind(sel_ct, panel.sel_row);
    acr_nav::FCtype *target = NULL;
    if (fld) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        target = reverse ? fld->p_ctype : fld->p_arg;
    }
    if (fld && target != sel_ct) {
        NavigateToTarget(sel_ct, target, acr_nav::_db.p_default_viewmode);
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_follow_ref() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*left);
    if (panel.position == 0) {
        FollowRefLeftPanel(left);
    } else if (acr_nav::_db.p_cur_viewmode->has_fields && sel_ct
               && panel.sel_row < RightPanelItemCount(sel_ct)) {
        FollowRefFields(panel, sel_ct);
    } else if (acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode) > 0
               && panel.sel_row < acr_nav::content_row_N(*acr_nav::_db.p_cur_viewmode)) {
        acr_nav::ContentRow &row = acr_nav::content_row_qFind(*acr_nav::_db.p_cur_viewmode, panel.sel_row);
        int col = i32_Min(acr_nav::_db.sel_nav_col, acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode) - 1);
        acr_nav::PreviewNavCol &nc = acr_nav::nav_col_qFind(*acr_nav::_db.p_cur_viewmode, col);
        algo::strptr target_key;
        if (acr_nav::nav_target_N(row) > 0 && col < acr_nav::nav_target_N(row)) {
            target_key = acr_nav::nav_target_qFind(row, col);
        } else if (acr_nav::nav_target_N(row) == 0) {
            target_key = nc.target_ctype;
        }
        if (ch_N(target_key) > 0) {
            acr_nav::FCtype *target = acr_nav::ind_ctype_Find(target_key);
            if (target && target != sel_ct) {
                tempstr cell_value;
                if (nc.col_wid > 0) {
                    algo::strptr row_text = RightPanelLineFind(panel.sel_row);
                    int start = DisplayToByte(row_text, nc.col_start);
                    int end = i32_Min(DisplayToByte(row_text, nc.col_start + nc.col_wid), elems_N(row_text));
                    if (start < elems_N(row_text)) {
                        algo::strptr raw_cell(row_text.elems + start, end - start);
                        cell_value << algo::TrimmedRight(raw_cell);
                    }
                }
                bool has_cell = ch_N(cell_value) > 0 || acr_nav::nav_target_N(row) > 0;
                if (has_cell && sel_ct) {
                    if (acr_nav::_db.p_cur_viewmode->pkey_wid > 0) {
                        acr_nav::_db.preview_nav_pending = cell_value;
                    }
                    NavigateToTarget(sel_ct, target, acr_nav::_db.p_cur_viewmode);
                } else if (has_cell) {
                    PushNaventry(acr_nav::_db.p_nsdep_ns
                        ? NsDisplayName(*acr_nav::_db.p_nsdep_ns)
                        : algo::strptr(""));
                    GoToCtype(target->ctype, acr_nav::_db.p_default_viewmode);
                }
            }
        }
    } else if (acr_nav::_db.p_cur_viewmode->scope_ns) {
        FollowRefNsDep(panel, left, sel_ct);
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_top() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    panel.sel_row = 0;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_bottom() {
    acr_nav::FPanel &panel = *acr_nav::_db.p_cur_panel;
    acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
    int n_items = PanelItemCount(panel, sel_ct);
    int last = i32_Max(0, n_items - 1);
    panel.sel_row = last;
}

// Restore filter, viewmode, navmode, and filtertarget from a navstack entry.
static void RestoreNavstackState(acr_nav::Naventry &entry) {
    acr_nav::_db.filter = entry.filter;
    acr_nav::_db.p_cur_viewmode = acr_nav::ind_viewmode_Find(entry.viewmode);
    acr_nav::_db.p_cur_mode = acr_nav::ind_navmode_Find(entry.navmode);
    acr_nav::_db.p_cur_filtertarget = acr_nav::ind_filtertarget_Find(entry.filtertarget);
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_go_back() {
    if (!acr_nav::navstack_EmptyQ()) {
        acr_nav::Naventry *entry = acr_nav::navstack_Last();
        RestoreNavstackState(*entry);
        // Ensure target namespace is expanded so the ctype is findable
        acr_nav::FCtype *target_ct = acr_nav::ind_ctype_Find(entry->ctype);
        if (target_ct) {
            target_ct->p_ns->collapsed = false;
        }
        BuildLeftItems();
        // Scan for the saved ctype -- collapse state may have changed since push
        acr_nav::_db.p_left_panel->sel_row = 0;
        acr_nav::_db.p_left_panel->scroll_offset = 0;
        if (target_ct) {
            int idx = FindLeftItemByCtype(entry->ctype);
            if (idx >= 0) {
                acr_nav::_db.p_left_panel->sel_row = idx;
            }
        } else {
            // No ctype match (nsdep namespace jump) — scan for namespace header
            // entry->ctype holds NsDisplayName: "other" for empty ns, else the ns key
            algo::strptr saved_name(entry->ctype);
            algo::strptr ns_key = (saved_name == "other") ? algo::strptr("") : saved_name;
            int idx = FindLeftItemByNsHeader(ns_key);
            if (idx >= 0) {
                acr_nav::_db.p_left_panel->sel_row = idx;
            }
        }
        acr_nav::_db.p_left_panel->scroll_offset = entry->scroll_offset;
        acr_nav::_db.p_right_panel->sel_row = entry->right_sel_row;
        acr_nav::_db.p_right_panel->scroll_offset = entry->right_scroll_offset;
        acr_nav::_db.sel_nav_col_pending = entry->sel_nav_col;
        acr_nav::_db.p_cur_panel = acr_nav::ind_panel_Find(entry->focus_panel);
        acr_nav::navstack_RemoveLast();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_quit() {
    acr_nav::_db.running = false;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_cycle_viewmode() {
    // In connect mode, stay on inspect viewmode
    if (!acr_nav::_db.live_mode) {
        acr_nav::FViewmode *next = acr_nav::ind_viewmode_Find(acr_nav::_db.p_cur_viewmode->next);
        if (next) {
            acr_nav::_db.p_cur_viewmode = next;
        }
    }
}

// -----------------------------------------------------------------------------

void acr_nav::ToggleViewmode(acr_nav::FViewmode *target) {
    bool in_target = (acr_nav::_db.p_cur_viewmode == target);
    bool can_enter = !in_target;
    if (can_enter && target->need_ssimfile) {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        can_enter = sel_ct && FindSsimfile(*sel_ct);
    }
    acr_nav::_db.p_cur_viewmode = can_enter ? target : acr_nav::_db.p_default_viewmode;
}

void acr_nav::navaction_toggle_preview() {
}
void acr_nav::navaction_toggle_codegen() {
}
void acr_nav::navaction_toggle_fields() {
}
void acr_nav::navaction_toggle_xref() {
}
void acr_nav::navaction_toggle_graph() {
}
void acr_nav::navaction_toggle_nsdep_detail() {
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_accept() {
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    if (has_filter) {
        // Recollapse previous session's auto-expanded before setting new ones
        RecollapseAutoExpanded();
        // Expand all namespaces that have matching ctypes so results are visible
        ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
            if (ns.n_match > 0 && ns.collapsed) {
                ns.auto_expanded = true;
                ns.collapsed = false;
            }
        } ind_end;
        BuildLeftItems();
        // Navigate cursor to first matching ctype (skip namespace headers)
        acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
        left->sel_row = 0;
        left->scroll_offset = 0;
        for (int i = 0; i < acr_nav::left_item_N(); i++) {
            if (ch_N(acr_nav::left_item_qFind(i).ctype) > 0) {
                left->sel_row = i;
                break;
            }
        }
    }
    SwitchToBrowse();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_start() {
    acr_nav::_db.pre_filter_text = acr_nav::_db.filter;
    acr_nav::_db.pre_filter_target = acr_nav::_db.p_cur_filtertarget->filtertarget;
    acr_nav::_db.pre_filter_sel_row = acr_nav::_db.p_left_panel->sel_row;
    acr_nav::_db.pre_filter_scroll_offset = acr_nav::_db.p_left_panel->scroll_offset;
    acr_nav::_db.p_cur_mode = acr_nav::_db.p_filter_mode;
    ch_RemoveAll(acr_nav::_db.filter);
    acr_nav::_db.p_cur_filtertarget = acr_nav::_db.p_default_filtertarget;
    BuildLeftItemsReset();
    acr_nav::_db.p_cur_panel = acr_nav::_db.p_left_panel;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_cancel() {
    SwitchToBrowse();
    acr_nav::_db.filter = acr_nav::_db.pre_filter_text;
    acr_nav::_db.p_cur_filtertarget = acr_nav::ind_filtertarget_Find(acr_nav::_db.pre_filter_target);
    BuildLeftItems();
    acr_nav::_db.p_left_panel->sel_row = acr_nav::_db.pre_filter_sel_row;
    acr_nav::_db.p_left_panel->scroll_offset = acr_nav::_db.pre_filter_scroll_offset;
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_append_space() {
    acr_nav::_db.filter << " ";
    BuildLeftItemsReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_clear() {
    if (ch_N(acr_nav::_db.filter) > 0) {
        ch_RemoveAll(acr_nav::_db.filter);
        RecollapseAutoExpanded();
        BuildLeftItemsReset();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_dismiss_or_clear() {
    if (!acr_nav::overlay_stack_EmptyQ()) {
        bool was_detail = IsDetailMode();
        PopOverlay();
        if (was_detail) {
            acr_nav::_db.p_detail_field = NULL;
        }
    } else {
        navaction_filter_clear();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_backspace() {
    if (ch_N(acr_nav::_db.filter) > 0) {
        acr_nav::_db.filter.ch_n = ch_N(acr_nav::_db.filter) - 1;
        BuildLeftItemsReset();
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_filter_cycle_target() {
    acr_nav::FFiltertarget *next = acr_nav::ind_filtertarget_Find(acr_nav::_db.p_cur_filtertarget->next);
    if (next) {
        acr_nav::_db.p_cur_filtertarget = next;
    }
    BuildLeftItemsReset();
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_help() {
    if (IsHelpMode()) {
        PopOverlay();
    } else {
        PushOverlay(acr_nav::ind_viewmode_Find("help"));
    }
}

// -----------------------------------------------------------------------------

void acr_nav::navaction_show_detail() {
    if (IsDetailMode()) {
        PopOverlay();
        acr_nav::_db.p_detail_field = NULL;
    } else if (acr_nav::_db.p_cur_viewmode->has_fields) {
        acr_nav::FCtype *sel_ct = SelectedCtype(*acr_nav::_db.p_left_panel);
        acr_nav::FField *fld = RightPanelFieldFind(sel_ct, acr_nav::_db.p_right_panel->sel_row);
        if (fld) {
            LoadDetail(*fld);
            PushOverlay(acr_nav::ind_viewmode_Find("detail"));
        }
    }
}
