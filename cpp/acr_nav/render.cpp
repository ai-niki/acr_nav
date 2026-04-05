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
// Source: cpp/acr_nav/render.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

static const char* G_VERT    = "\xe2\x94\x82";  // │  single vertical
static const char* G_CHEVRON = "\xe2\x80\xba";  // ›  breadcrumb separator
static const char* G_SEP     = " \xe2\x94\x82 ";  // " │ " status bar section separator
static const int   G_SEP_DISP = 3;                 // display columns for G_SEP

using acr_nav::SelectedNs;
using acr_nav::RightPanelItemCount;
using acr_nav::RightPanelLineFind;
using acr_nav::RightPanelLineHeader;
using acr_nav::RightPanelFieldFind;
using acr_nav::PanelItemCount;
using acr_nav::FieldMatchesFilter;
using acr_nav::IsDetailMode;
using acr_nav::GraphInfoAtLine;
using acr_nav::NsDisplayName;
using acr_nav::PrintRecordCount;
using acr_nav::DecimalDigits;
using acr_nav::DisplayToByte;
using acr_nav::Utf8ExtraBytes;
using acr_nav::VisibleRows;
using acr_nav::AdjustScroll;
using acr_nav::BuildBreadcrumb;
using acr_nav::BuildStatusHint;

// Attributes compose: caller may emit selection style then field color.
static void EmitStyle(cstring &out, acr_nav::FNavstyle &style) {
    if (style.bold) out << "\x1b[1m";
    if (style.dim) out << "\x1b[2m";
    if (style.reverse) out << "\x1b[7m";
    if (style.fg_color != algo_TermColor_default) {
        out << "\x1b[" << (90 + u32(style.fg_color)) << "m";
    }
}

// Truncate str to (max_width - reserve) display columns, then right-pad with
// spaces so the total padding + content = max_width - reserve columns.
// UTF-8-aware: does not split multi-byte characters.
static void TruncPad(cstring &str, int max_width, int reserve = 0) {
    int available = max_width - reserve;
    int display_width = ch_N(str) - Utf8ExtraBytes(strptr(str));
    if (available > 0 && display_width > available) {
        str.ch_n = DisplayToByte(strptr(str), available);
        display_width = available;
    } else if (available <= 0) {
        str.ch_n = 0;
        display_width = 0;
    }
    char_PrintNTimes(' ', str, i32_Max(0, max_width - reserve - display_width));
}

// -----------------------------------------------------------------------------

// Per-frame rendering context.  Populated once by Render, read by phase functions.
struct RenderCtx {
    cstring            &buf;
    acr_nav::FCtype    *sel_ct;
    int                 wid;
    int                 left_wid;
    int                 right_wid;
    bool                left_focused;
    bool                show_breadcrumb;
    int                 visible;
    bool                has_fields;
    bool                in_xref;
    int                 n_left;
    int                 n_right;
    int                 scroll;
    int                 preview_h_scroll;
    bool                is_hscroll;
    acr_nav::FNavstyle *p_title_focus;
    acr_nav::FNavstyle *p_title_nofocus;
    acr_nav::FNavstyle *p_sel_focus;
    acr_nav::FNavstyle *p_sel_nofocus;
    acr_nav::FNavstyle *p_statusbar;
    RenderCtx(cstring &buf_
              , acr_nav::FCtype *sel_ct_
              , int wid_
              , int left_wid_
              , int right_wid_
              , bool left_focused_
              , bool show_breadcrumb_
              , int visible_
              , bool has_fields_
              , bool in_xref_
              , int n_left_
              , int n_right_
              , int scroll_
              , int preview_h_scroll_
              , bool is_hscroll_
              , acr_nav::FNavstyle *p_title_focus_
              , acr_nav::FNavstyle *p_title_nofocus_
              , acr_nav::FNavstyle *p_sel_focus_
              , acr_nav::FNavstyle *p_sel_nofocus_
              , acr_nav::FNavstyle *p_statusbar_)
        : buf(buf_), sel_ct(sel_ct_)
        , wid(wid_), left_wid(left_wid_), right_wid(right_wid_)
        , left_focused(left_focused_), show_breadcrumb(show_breadcrumb_), visible(visible_)
        , has_fields(has_fields_), in_xref(in_xref_)
        , n_left(n_left_), n_right(n_right_), scroll(scroll_)
        , preview_h_scroll(preview_h_scroll_), is_hscroll(is_hscroll_)
        , p_title_focus(p_title_focus_), p_title_nofocus(p_title_nofocus_)
        , p_sel_focus(p_sel_focus_), p_sel_nofocus(p_sel_nofocus_)
        , p_statusbar(p_statusbar_)
    {}
};

// -----------------------------------------------------------------------------

// Build breadcrumb trail from navigation stack.
// Returns empty string at depth 0, or "A > B > C" showing the path of ctypes visited.
tempstr acr_nav::BuildBreadcrumb(acr_nav::FCtype *sel_ct) {
    tempstr bc;
    for (int i = 0; i < acr_nav::navstack_N(); i++) {
        if (i > 0) {
            bc << " " << G_CHEVRON << " ";
        }
        bc << acr_nav::navstack_qFind(i).ctype;
    }
    if (acr_nav::navstack_N() > 0) {
        if (sel_ct) {
            bc << " " << G_CHEVRON << " " << sel_ct->ctype;
        } else {
            acr_nav::FNs *ns = SelectedNs();
            if (ns) {
                bc << " " << G_CHEVRON << " " << NsDisplayName(*ns);
            }
        }
    }
    return bc;
}

// Build status bar hint from explicit strings on navmode/viewmode.
// Filter: navmode.status_hint. Overlay: viewmode.status_hint.
// Non-overlay browse: arrow indicators + optional "Backspace:back" +
// viewmode.status_hint (right panel) or navmode.status_hint (left panel).
void acr_nav::BuildStatusHint(cstring &out) {
    bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
    acr_nav::FViewmode &vm = *acr_nav::_db.p_cur_viewmode;
    if (in_filter) {
        out << acr_nav::_db.p_cur_mode->status_hint;
    } else if (vm.is_overlay) {
        out << vm.status_hint;
    } else {
        bool on_right = (acr_nav::_db.p_cur_panel->position == 1);
        out << "\xe2\x86\x91\xe2\x86\x93\xe2\x86\x90\xe2\x86\x92"; // ↑↓←→
        if (!acr_nav::navstack_EmptyQ()) {
            out << "  Backspace:back";
        }
        algo::strptr body = on_right ? algo::strptr(vm.status_hint)
                                     : algo::strptr(acr_nav::_db.p_cur_mode->status_hint);
        if (ch_N(body) > 0) {
            out << "  " << body;
        }
    }
}

// -----------------------------------------------------------------------------

// Render the title bar: left panel title | right panel title.
static void RenderTitleBar(RenderCtx &ctx) {
    // Left panel
    {
        tempstr ltitle;
        ltitle << " " << acr_nav::_db.p_left_panel->title << " (" << acr_nav::_db.n_visible_ctype << ")";
        TruncPad(ltitle, ctx.left_wid - 1);
        EmitStyle(ctx.buf, ctx.left_focused ? *ctx.p_title_focus : *ctx.p_title_nofocus);
        ctx.buf << ltitle << "\x1b[0m";
    }
    ctx.buf << G_VERT;
    // Right panel title: detail mode shows field name instead of ctype name.
    // Title branches: detail mode, has_fields+selected, has_fields+empty, line-mode.
    // has_fields on FViewmode is the structural axis separating field/line rendering.
    {
        tempstr rtitle;
        if (IsDetailMode() && acr_nav::_db.p_detail_field) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << ": " << acr_nav::_db.p_detail_field->field
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        } else if (acr_nav::_db.p_cur_viewmode->has_fields && ctx.sel_ct) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << ": " << ctx.sel_ct->ctype
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        } else if (acr_nav::_db.p_cur_viewmode->has_fields) {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title;
        } else {
            rtitle << " " << acr_nav::_db.p_cur_viewmode->title
                   << " (" << RightPanelItemCount(ctx.sel_ct) << ")";
        }
        TruncPad(rtitle, ctx.right_wid);
        EmitStyle(ctx.buf, !ctx.left_focused ? *ctx.p_title_focus : *ctx.p_title_nofocus);
        ctx.buf << rtitle << "\x1b[0m";
    }
    ctx.buf << "\r\n";
}

// -----------------------------------------------------------------------------

// Emit preview header with cyan-highlighted navigable (FK) column names.
// nav_col entries provide column positions; non-nav regions get base_style.
// h_scroll shifts column positions left for horizontal scrolling.
static void EmitStyledPreviewHeader(cstring &buf, algo::strptr hdr, acr_nav::FViewmode &vm, acr_nav::FNavstyle &base_style, int h_scroll = 0) {
    int prev_end = 0;
    int hdr_n = elems_N(hdr);
    int n_nav = acr_nav::nav_col_N(vm);
    for (int ni = 0; ni < n_nav; ni++) {
        acr_nav::PreviewNavCol &nc = acr_nav::nav_col_qFind(vm, ni);
        if (ch_N(nc.target_ctype) == 0) {
            continue;  // non-FK column, no highlight -- inside loop, not function return
        }
        int adj_start = nc.col_start - h_scroll + 1;  // +1 for leading space in right_cell
        int adj_end = i32_Min(adj_start + nc.name_len, hdr_n);
        if (adj_end <= 1) {
            continue;  // entirely off-screen left -- inside loop, not function return
        }
        if (adj_start < 1) {
            adj_start = 1;
        }
        if (adj_start >= hdr_n) {
            continue;  // inside loop, not function return
        }
        // Plain run before this nav column
        if (adj_start > prev_end) {
            buf << algo::strptr(hdr.elems + prev_end, adj_start - prev_end);
        }
        // Cyan-styled nav column name
        acr_nav::FNavstyle *line_nav_header = acr_nav::ind_navstyle_Find("line_nav_header");
        if (line_nav_header) {
            EmitStyle(buf, *line_nav_header);
        }
        buf << algo::strptr(hdr.elems + adj_start, adj_end - adj_start);
        buf << "\x1b[0m";
        EmitStyle(buf, base_style);
        prev_end = adj_end;
    }
    // Trailing plain run
    if (prev_end < hdr_n) {
        buf << algo::strptr(hdr.elems + prev_end, hdr_n - prev_end);
    }
}

// Emit a text region [rs, re) from right_cell, splitting at overlay boundaries.
// base_style is applied to non-overlay portions (NULL for plain text).
// overlay_style replaces base_style inside [ov_start, ov_end).
static void EmitRegionWithOverlay(cstring &buf, algo::strptr right_cell, int rs, int re, bool right_sel, acr_nav::FNavstyle *sel_style, acr_nav::FNavstyle *base_style, int ov_start, int ov_end, acr_nav::FNavstyle *overlay_style) {
    // Before overlay
    if (rs < ov_start && rs < re) {
        int seg_end = i32_Min(ov_start, re);
        if (base_style) {
            EmitStyle(buf, *base_style);
        }
        buf << algo::strptr(right_cell.elems + rs, seg_end - rs);
        if (base_style) {
            buf << "\x1b[0m";
            if (right_sel && sel_style) {
                EmitStyle(buf, *sel_style);
            }
        }
        rs = seg_end;
    }
    // Inside overlay
    if (rs >= ov_start && rs < ov_end && rs < re && overlay_style) {
        int seg_end = i32_Min(ov_end, re);
        buf << "\x1b[0m";
        EmitStyle(buf, *overlay_style);
        buf << algo::strptr(right_cell.elems + rs, seg_end - rs);
        buf << "\x1b[0m";
        if (right_sel && sel_style) {
            EmitStyle(buf, *sel_style);
        }
        rs = seg_end;
    }
    // After overlay
    if (rs < re) {
        if (base_style) {
            EmitStyle(buf, *base_style);
        }
        buf << algo::strptr(right_cell.elems + rs, re - rs);
        if (base_style) {
            buf << "\x1b[0m";
            if (right_sel && sel_style) {
                EmitStyle(buf, *sel_style);
            }
        }
    }
}

// Emit a line with color spans interleaved. Spans reference positions in the stored line text
// (0-based). right_cell has a leading space prefix, so span positions are adjusted by +1.
// Uses span-boundary runs for clean SESE flow. span_cursor advances monotonically across
// visible lines for O(visible) total scan cost.
// overlay_start/overlay_end define a region where overlay_style replaces base styles.
static void EmitStyledLine(cstring &buf, algo::strptr right_cell, bool right_sel, acr_nav::FViewmode &vm, int line_idx, int &span_cursor, acr_nav::FNavstyle *sel_style, int overlay_start, int overlay_end, acr_nav::FNavstyle *overlay_style, int skip_bytes = 0) {
    int cell_n = elems_N(right_cell);
    int prev_end = 0;
    int ov_start = (overlay_start >= 0 && overlay_style) ? overlay_start + 1 : cell_n + 1;
    int ov_end = (overlay_end >= 0 && overlay_style) ? i32_Min(overlay_end + 1, cell_n) : cell_n + 1;
    // Advance span_cursor to first span for this line
    int n_spans = acr_nav::cspan_N(vm);
    while (span_cursor < n_spans && acr_nav::cspan_qFind(vm, span_cursor).line_idx < line_idx) {
        span_cursor++;
    }
    // Process spans for this line
    int si = span_cursor;
    while (si < n_spans && acr_nav::cspan_qFind(vm, si).line_idx == line_idx) {
        acr_nav::LineColorSpan &span = acr_nav::cspan_qFind(vm, si);
        int adj_start = span.col_start - skip_bytes + 1;
        int adj_end = i32_Min(span.col_end - skip_bytes + 1, cell_n);
        if (adj_end <= 1) {
            si++;
            continue;  // span entirely off-screen left -- inside while loop, not function-level return
        }
        if (adj_start < 1) {
            adj_start = 1;
        }
        if (adj_start >= cell_n) {
            si++;
            continue;  // span fully clipped -- inside while loop, not function-level return
        }
        // Emit plain run before this span
        if (adj_start > prev_end) {
            EmitRegionWithOverlay(buf, right_cell, prev_end, adj_start, right_sel, sel_style, nullptr, ov_start, ov_end, overlay_style);
        }
        // Emit styled span
        EmitRegionWithOverlay(buf, right_cell, adj_start, adj_end, right_sel, sel_style, span.p_navstyle, ov_start, ov_end, overlay_style);
        prev_end = adj_end;
        si++;
    }
    // Emit trailing plain run
    if (prev_end < cell_n) {
        EmitRegionWithOverlay(buf, right_cell, prev_end, cell_n, right_sel, sel_style, nullptr, ov_start, ov_end, overlay_style);
    }
}

// -----------------------------------------------------------------------------

// Render column header row above data: line-mode header with h-scroll,
// field-mode header with field/arg/reftype columns, or empty.
static void RenderColumnHeader(RenderCtx &ctx) {
    tempstr left_cell;
    char_PrintNTimes(' ', left_cell, ctx.left_wid - 1);
    ctx.buf << left_cell << "\x1b[0m" << G_VERT;
    tempstr hdr;
    if (!ctx.has_fields) {
        algo::strptr line_header = RightPanelLineHeader();
        if (ch_N(line_header) > 0) {
            if (ctx.is_hscroll) {
                int hdr_skip = DisplayToByte(line_header, ctx.preview_h_scroll);
                hdr << " " << algo::strptr(line_header.elems + hdr_skip,
                                            line_header.n_elems - hdr_skip);
            } else {
                hdr << " " << line_header;
            }
        }
    } else if (ctx.sel_ct) {
        hdr << " field";
        char_PrintNTimes(' ', hdr, i32_Max(1, 24 - ch_N(hdr)));
        hdr << (ctx.in_xref ? "ctype" : "arg");
        char_PrintNTimes(' ', hdr, i32_Max(1, 48 - ch_N(hdr)));
        hdr << "reftype";
    }
    TruncPad(hdr, ctx.right_wid);
    acr_nav::FNavstyle &base_hdr_style = !ctx.left_focused ? *ctx.p_title_focus : *ctx.p_title_nofocus;
    EmitStyle(ctx.buf, base_hdr_style);
    if (!ctx.has_fields && acr_nav::nav_col_N(*acr_nav::_db.p_cur_viewmode) > 0) {
        EmitStyledPreviewHeader(ctx.buf, strptr(hdr), *acr_nav::_db.p_cur_viewmode, base_hdr_style, ctx.preview_h_scroll);
    } else {
        ctx.buf << hdr;
    }
    ctx.buf << "\x1b[0m\x1b[K\r\n";
}

// Render one left-panel cell: namespace header with collapse icon + count,
// or ctype row with indented name + record count.  Applies selection highlight.
static void RenderLeftCell(RenderCtx &ctx, int row) {
    tempstr left_cell;
    bool left_sel = false;
    int left_idx = ctx.scroll + row;
    if (left_idx < ctx.n_left) {
        left_sel = (left_idx == acr_nav::_db.p_left_panel->sel_row);
        acr_nav::LeftItem &item = acr_nav::left_item_qFind(left_idx);
        bool live_mode = acr_nav::_db.live_mode;
        if (ch_N(item.ctype) == 0) {
            // Namespace header row
            acr_nav::FNs *ns = acr_nav::ind_ns_Find(item.ns);
            if (live_mode) {
                // Live mode: always expanded, show pool count
                left_cell << " \xe2\x96\xbe " << acr_nav::_db.live_ns;
                left_cell << " (" << acr_nav::pool_entry_N() << " pools)";
            } else {
                int count = ns ? ns->n_match : 0;
                left_cell << (ns && ns->collapsed ? " \xe2\x96\xb8 " : " \xe2\x96\xbe ");
                left_cell << (ns ? NsDisplayName(*ns) : algo::strptr("other"));
                left_cell << " (" << count << ")";
            }
        } else {
            // Ctype row: indented, namespace prefix stripped
            algo::strptr full(item.ctype);
            algo::strptr stripped = algo::Pathcomp(full, ".LR");
            // Dot-less ctypes (extern types): use full key as name
            if (elems_N(stripped) == 0) {
                stripped = full;
            }
            left_cell << "    " << stripped;
            if (live_mode) {
                left_cell << " (" << item.n_record << ")";
            } else {
                acr_nav::FCtype *ct = acr_nav::ind_ctype_Find(item.ctype);
                if (ct) {
                    PrintRecordCount(left_cell, *ct);
                }
            }
        }
    }
    TruncPad(left_cell, ctx.left_wid - 1);
    if (left_sel) {
        EmitStyle(ctx.buf, ctx.left_focused ? *ctx.p_sel_focus : *ctx.p_sel_nofocus);
    }
    ctx.buf << left_cell << "\x1b[0m" << G_VERT;
}

// Compute the preview nav-cell overlay region for the selected row.
// Sets ov_start/ov_end to byte offsets within the right cell, *p_ov_style to the
// overlay style.  When no overlay applies, ov_start and ov_end remain -1.
static void DetectNavOverlay(RenderCtx &ctx, int right_data_idx, int skip_bytes, bool right_sel, int &ov_start, int &ov_end, acr_nav::FNavstyle **p_ov_style) {
    ov_start = -1;
    ov_end = -1;
    *p_ov_style = nullptr;
    acr_nav::FViewmode &vm = *acr_nav::_db.p_cur_viewmode;
    if (right_sel && !ctx.left_focused
        && acr_nav::nav_col_N(vm) > 0) {
        int n_nav = acr_nav::nav_col_N(vm);
        if (acr_nav::_db.sel_nav_col < n_nav) {
            acr_nav::PreviewNavCol &nc = acr_nav::nav_col_qFind(vm, acr_nav::_db.sel_nav_col);
            algo::strptr data_line = RightPanelLineFind(right_data_idx);
            int ov_raw_s = DisplayToByte(data_line, nc.col_start);
            int ov_raw_e = DisplayToByte(data_line, nc.col_start + nc.col_wid);
            ov_start = ov_raw_s - skip_bytes;
            ov_end = ov_raw_e - skip_bytes;
            *p_ov_style = ch_N(nc.target_ctype) > 0
                ? acr_nav::ind_navstyle_Find("line_nav_cell")
                : acr_nav::ind_navstyle_Find("line_nav_cell_nofk");
        }
    }
}

// Render one right-panel cell: line-mode with h-scroll, field-mode columns,
// or empty message.  Applies selection, reftype color, filter-match highlight,
// and dispatches to EmitStyledLine for span-mode content.
static void RenderRightCell(RenderCtx &ctx, int row, int &span_cursor) {
    acr_nav::FNavstyle *filter_match = acr_nav::ind_navstyle_Find("filter_match");
    tempstr right_cell;
    bool right_sel = false;
    int right_data_idx = acr_nav::_db.p_right_panel->scroll_offset + row;
    int skip_bytes = 0;
    acr_nav::FField *fld = nullptr;
    if ((ctx.sel_ct || !ctx.has_fields) && right_data_idx < ctx.n_right) {
        right_sel = (right_data_idx == acr_nav::_db.p_right_panel->sel_row);
        if (!ctx.has_fields) {
            algo::strptr orig_line = RightPanelLineFind(right_data_idx);
            if (ctx.is_hscroll) {
                skip_bytes = DisplayToByte(orig_line, ctx.preview_h_scroll);
                right_cell << " " << algo::strptr(orig_line.elems + skip_bytes,
                                                   orig_line.n_elems - skip_bytes);
            } else {
                right_cell << " " << orig_line;
            }
        } else {
            fld = RightPanelFieldFind(ctx.sel_ct, right_data_idx);
            if (fld) {
                right_cell << " " << name_Get(*fld);
                char_PrintNTimes(' ', right_cell, i32_Max(1, 24 - ch_N(right_cell)));
                right_cell << (ctx.in_xref ? fld->p_ctype->ctype : fld->p_arg->ctype);
                char_PrintNTimes(' ', right_cell, i32_Max(1, 48 - ch_N(right_cell)));
                right_cell << fld->p_reftype->reftype;
            }
        }
    } else if ((ctx.sel_ct || !ctx.has_fields) && ctx.n_right == 0 && right_data_idx == 0) {
        right_cell << " (" << acr_nav::_db.p_cur_viewmode->empty_msg << ")";
    }
    TruncPad(right_cell, ctx.right_wid);
    if (right_sel && !ctx.left_focused) {
        EmitStyle(ctx.buf, *ctx.p_sel_focus);
    }
    if (fld && fld->p_reftype->c_reftypestyle) {
        EmitStyle(ctx.buf, *fld->p_reftype->c_reftypestyle->p_navstyle);
    }
    bool field_match = false;
    if (fld && !ctx.in_xref
        && acr_nav::_db.p_cur_filtertarget->has_field_criteria
        && ch_N(acr_nav::_db.filter) > 0) {
        field_match = FieldMatchesFilter(*fld, acr_nav::_db.filter_regx, *acr_nav::_db.p_cur_filtertarget);
    }
    if (field_match) {
        EmitStyle(ctx.buf, *filter_match);
    }
    if (!ctx.has_fields && acr_nav::cspan_N(*acr_nav::_db.p_cur_viewmode) > 0) {
        bool right_focused_sel = right_sel && !ctx.left_focused;
        int ov_start = -1;
        int ov_end = -1;
        acr_nav::FNavstyle *ov_style = nullptr;
        DetectNavOverlay(ctx, right_data_idx, skip_bytes, right_sel, ov_start, ov_end, &ov_style);
        EmitStyledLine(ctx.buf, strptr(right_cell), right_focused_sel, *acr_nav::_db.p_cur_viewmode, right_data_idx, span_cursor, right_focused_sel ? ctx.p_sel_focus : nullptr, ov_start, ov_end, ov_style, skip_bytes);
        ctx.buf << "\x1b[0m\x1b[K\r\n";
    } else {
        ctx.buf << right_cell << "\x1b[0m\x1b[K\r\n";
    }
}

// Render the content area: column header + dual-panel data rows.
static void RenderContentArea(RenderCtx &ctx) {
    RenderColumnHeader(ctx);
    int data_rows = ctx.visible - 1;
    int span_cursor = 0;
    for (int row = 0; row < data_rows; row++) {
        RenderLeftCell(ctx, row);
        RenderRightCell(ctx, row, span_cursor);
    }
}

// -----------------------------------------------------------------------------

// Render the breadcrumb bar (shown only when navstack is non-empty).
static void RenderBreadcrumbBar(RenderCtx &ctx) {
    if (ctx.show_breadcrumb) {
        tempstr bcline;
        bcline << " " << BuildBreadcrumb(ctx.sel_ct);
        TruncPad(bcline, ctx.wid);
        EmitStyle(ctx.buf, *ctx.p_statusbar);
        ctx.buf << bcline << "\x1b[0m\r\n";
    }
}

// -----------------------------------------------------------------------------

// Render the status bar: filter text + hints + position indicator.
static void RenderStatusBar(RenderCtx &ctx) {
    EmitStyle(ctx.buf, *ctx.p_statusbar);
    tempstr status;
    bool in_filter = (acr_nav::_db.p_cur_mode == acr_nav::_db.p_filter_mode);
    bool has_filter = ch_N(acr_nav::_db.filter) > 0;
    status << " ";
    if (in_filter || has_filter) {
        status << "/";
        if (ch_N(acr_nav::_db.p_cur_filtertarget->label) > 0) {
            status << acr_nav::_db.p_cur_filtertarget->label;
        }
        status << acr_nav::_db.filter;
    }
    if (in_filter && ch_N(acr_nav::_db.p_cur_filtertarget->description) > 0) {
        status << "  (" << acr_nav::_db.p_cur_filtertarget->description << ")";
    }
    if (in_filter || has_filter) {
        status << "  ";
    }
    BuildStatusHint(status);
    acr_nav::FPanel &cur = *acr_nav::_db.p_cur_panel;
    acr_nav::FViewmode &graph_vm = *acr_nav::ind_viewmode_Find("graph");
    acr_nav::FCtype *graph_ct = ch_N(graph_vm.cached_key) > 0
        ? acr_nav::ind_ctype_Find(graph_vm.cached_key) : nullptr;
    if (acr_nav::_db.p_cur_viewmode == &graph_vm
        && graph_ct
        && cur.position == 1) {
        acr_nav::FField *fld = NULL;
        GraphInfoAtLine(*graph_ct, cur.sel_row, NULL, &fld);
        if (fld && ch_N(fld->p_reftype->comment) > 0) {
            status << G_SEP << fld->p_reftype->comment;
        }
    }
    int cur_items = PanelItemCount(cur, ctx.sel_ct);
    tempstr pos;
    if (cur_items > 0) {
        pos << (cur.sel_row + 1) << "/" << cur_items;
    } else {
        pos << "0/0";
    }
    int pos_section = G_SEP_DISP + ch_N(pos);
    TruncPad(status, ctx.wid, pos_section);
    status << G_SEP << pos;
    ctx.buf << status << "\x1b[0m";
}

// Adjust horizontal scroll offset to keep the selected preview column visible.
// Scroll offset always lands on a column boundary: the leftmost visible column
// starts at position 0 with no partial-column whitespace on the left.
// h only changes when the selected column is not fully visible.
static void AdjustHScroll(int right_wid) {
    acr_nav::FViewmode &vm = *acr_nav::_db.p_cur_viewmode;
    int h = vm.preview_h_scroll;
    int n_nav = acr_nav::nav_col_N(vm);
    if (n_nav > 0) {
        int avail = right_wid - 1;  // -1 for leading space in right_cell
        if (vm.total_content_wid > avail && avail > 0) {
            int sel = i32_Min(acr_nav::_db.sel_nav_col, n_nav - 1);
            acr_nav::PreviewNavCol &nc = acr_nav::nav_col_qFind(vm, sel);
            // Selected column off-screen left: snap to its start
            if (nc.col_start < h) {
                h = nc.col_start;
            }
            // Selected column off-screen right: find the smallest column
            // boundary that makes the selected column's right edge visible
            if (nc.col_start + nc.col_wid > h + avail) {
                int min_start = nc.col_start + nc.col_wid - avail;
                h = nc.col_start;  // fallback: start at selected column
                for (int i = 0; i < n_nav; i++) {
                    int cs = acr_nav::nav_col_qFind(vm, i).col_start;
                    if (cs >= min_start) {
                        h = i32_Min(cs, nc.col_start);
                        break;
                    }
                }
            }
            int max_scroll = vm.total_content_wid - avail;
            h = i32_Max(0, i32_Min(h, max_scroll));
        } else {
            h = 0;
        }
    } else {
        h = 0;
    }
    vm.preview_h_scroll = h;
}

// -----------------------------------------------------------------------------

void acr_nav::Render(cstring &buf, acr_nav::FCtype *sel_ct) {
    acr_nav::FPanel *left = acr_nav::_db.p_left_panel;
    AdjustScroll(*left, acr_nav::left_item_N());
    AdjustScroll(*acr_nav::_db.p_right_panel, RightPanelItemCount(sel_ct));
    buf << "\x1b[H";
    int wid = acr_nav::_db.term_wid;
    // Left panel width: fits longest row across all matching namespaces
    // (regardless of collapse state, so width is stable on expand/collapse)
    int max_name = 0;
    if (acr_nav::_db.live_mode) {
        // Live mode: compute width from left_item array (PoolCensus-driven)
        for (int i = 0; i < acr_nav::left_item_N(); i++) {
            acr_nav::LeftItem &item = acr_nav::left_item_qFind(i);
            if (ch_N(item.ctype) == 0) {
                // Namespace header: " V ns (N pools)"
                int hdr_wid = 4 + ch_N(acr_nav::_db.live_ns) + 2 + DecimalDigits(acr_nav::pool_entry_N()) + 7;
                max_name = i32_Max(max_name, hdr_wid);
            } else {
                // Pool row: "    TypeName (N)"
                algo::strptr stripped = algo::Pathcomp(algo::strptr(item.ctype), ".LR");
                int count_wid = 3 + DecimalDigits(item.n_record);
                max_name = i32_Max(max_name, 4 + elems_N(stripped) + count_wid);
            }
        }
    } else {
        ind_beg(acr_nav::_db_ns_curs, ns, acr_nav::_db) {
            if (ns.n_match > 0) {
                // Namespace header: " X label (count)"
                int label_len = ch_N(NsDisplayName(ns));
                int count = ns.n_match;
                int hdr_wid = 4 + label_len + 3 + DecimalDigits(count);
                max_name = i32_Max(max_name, hdr_wid);
                // Ctype rows: "    TypeName (N)" = 4 + stripped name + count width
                for (int i = 0; i < acr_nav::c_ctype_N(ns); i++) {
                    acr_nav::FCtype *ct = acr_nav::c_ctype_Find(ns, i);
                    if (ct && ch_N(ct->ctype) > 0) {
                        algo::strptr stripped = algo::Pathcomp(ct->ctype, ".LR");
                        int count_wid = 0;
                        if (ct->c_ssimfile && ct->c_ssimfile->n_record > 0) {
                            count_wid = 3 + DecimalDigits(ct->c_ssimfile->n_record);
                        }
                        max_name = i32_Max(max_name, 4 + elems_N(stripped) + count_wid);
                    }
                }
            }
        } ind_end;
    }
    int min_left = i32_Min(acr_nav::_db.p_left_panel->min_width, wid / 2);
    int left_wid = i32_Max(min_left, i32_Min(max_name + 2, wid * 40 / 100));
    int right_wid = i32_Max(1, wid - left_wid);
    AdjustHScroll(right_wid);
    bool has_fields = acr_nav::_db.p_cur_viewmode->has_fields;
    acr_nav::FViewmode &cur_vm = *acr_nav::_db.p_cur_viewmode;
    int preview_h_scroll = 0;
    bool is_hscroll = (!has_fields
        && acr_nav::nav_col_N(cur_vm) > 0
        && cur_vm.preview_h_scroll > 0);
    if (is_hscroll) {
        preview_h_scroll = cur_vm.preview_h_scroll;
    }
    acr_nav::FNavstyle *p_title_focus   = acr_nav::ind_navstyle_Find("title_focus");
    acr_nav::FNavstyle *p_title_nofocus = acr_nav::ind_navstyle_Find("title_nofocus");
    acr_nav::FNavstyle *p_sel_focus     = acr_nav::ind_navstyle_Find("sel_focus");
    acr_nav::FNavstyle *p_sel_nofocus   = acr_nav::ind_navstyle_Find("sel_nofocus");
    acr_nav::FNavstyle *p_statusbar     = acr_nav::ind_navstyle_Find("statusbar");
    RenderCtx ctx(buf, sel_ct
                  , wid, left_wid, right_wid
                  , /*left_focused=*/(acr_nav::_db.p_cur_panel == acr_nav::_db.p_left_panel)
                  , /*show_breadcrumb=*/(acr_nav::navstack_N() > 0)
                  , /*visible=*/VisibleRows()
                  , /*has_fields=*/has_fields
                  , /*in_xref=*/acr_nav::_db.p_cur_viewmode->is_reverse
                  , /*n_left=*/acr_nav::left_item_N()
                  , /*n_right=*/RightPanelItemCount(sel_ct)
                  , /*scroll=*/acr_nav::_db.p_left_panel->scroll_offset
                  , /*preview_h_scroll=*/preview_h_scroll
                  , /*is_hscroll=*/is_hscroll
                  , p_title_focus, p_title_nofocus, p_sel_focus, p_sel_nofocus, p_statusbar);
    RenderTitleBar(ctx);
    RenderContentArea(ctx);
    RenderBreadcrumbBar(ctx);
    RenderStatusBar(ctx);
}
