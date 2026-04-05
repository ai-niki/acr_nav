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
// Source: cpp/acr_nav/content.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

static const char* G_HORIZ = "\xe2\x94\x80";  // ─  single horizontal

using acr_nav::AddSpan;
using acr_nav::ClearViewmodeLines;
using acr_nav::FindSsimfile;
using acr_nav::SanitizeForDisplay;
using acr_nav::Utf8ExtraBytes;
using acr_nav::DisplayToByte;
using acr_nav::NsDisplayName;
using acr_nav::IsNsDepMode;
using acr_nav::SelectedNs;
using acr_nav::RightPanelLineCount;
using acr_nav::DecimalDigits;

// Load ssimfile content into the preview viewmode's line Tary, stripping the tuple head from each line.
// Format a single row of attr values into an aligned column string.
static void FormatPreviewRow(cstring &out, algo::Tuple &tuple, int *display_wid, int n_col, int *col_byte_pos) {
    int ci = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        if (ci < n_col) {
            if (ci > 0) {
                out << "  ";
            }
            if (col_byte_pos) col_byte_pos[ci] = ch_N(out);
            tempstr safe(attr.value);
            SanitizeForDisplay(safe);
            out << safe;
            int disp_len = i32_Max(0, ch_N(safe) - Utf8ExtraBytes(strptr(safe)));
            char_PrintNTimes(' ', out, display_wid[ci] - disp_len);
            ci++;
        }
    } ind_end;
    // Fill missing columns
    while (ci < n_col) {
        if (ci > 0) {
            out << "  ";
        }
        if (col_byte_pos) col_byte_pos[ci] = ch_N(out);
        char_PrintNTimes(' ', out, display_wid[ci]);
        ci++;
    }
}

// Measure one tuple's contribution to column names and widths.
// On first call (n_col==0), populate col_name from attribute names.
// On every call, update display_wid with max attribute display widths.
static void MeasureTupleColumns(algo::Tuple &tuple, algo::cstring *col_name, int *display_wid, int &n_col) {
    if (n_col == 0) {
        ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
            if (n_col < 64) {
                col_name[n_col] = attr.name;
                display_wid[n_col] = ch_N(attr.name);
                n_col++;
            }
        } ind_end;
    }
    int ci = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        if (ci < n_col) {
            display_wid[ci] = i32_Max(display_wid[ci], ch_N(attr.value) - Utf8ExtraBytes(strptr(attr.value)));
            ci++;
        }
    } ind_end;
}

// First-pass scan of mmap'd ssimfile: determine column names from the first tuple,
// compute max display widths across all rows.
static void MeasurePreviewColumns(algo_lib::MmapFile &file, algo::cstring *col_name, int *display_wid, int &n_col) {
    n_col = 0;
    ind_beg(Line_curs, line, file.text) {
        algo::Tuple tuple;
        if (algo::Tuple_ReadStrptr(tuple, line, false)) {
            MeasureTupleColumns(tuple, col_name, display_wid, n_col);
        }
    } ind_end;
}

// Check whether a ctype appears as a pool in the live state dump.
static bool FindPoolEntry(algo::strptr ctype_key) {
    for (int i = 0; i < acr_nav::pool_entry_N(); i++) {
        if (algo::strptr_Eq(acr_nav::pool_entry_qFind(i).ctype, ctype_key)) {
            return true;
        }
    }
    return false;
}

// Build PreviewNavCol entries for each column, detect FK targets for navigable columns.
// live_mode: FK detection checks pool_entry membership (Ptr fields navigable).
// !live_mode: FK detection checks reftype.up and ssimfile existence.
static void DetectNavColumns(acr_nav::FViewmode &vm, acr_nav::FCtype *field_base, algo::cstring *col_name, int *display_wid, int n_col, bool live_mode) {
    int col_pos = 0;
    for (int c = 0; c < n_col; c++) {
        if (c > 0) {
            col_pos += 2; // separator
        }
        acr_nav::PreviewNavCol &nc = acr_nav::nav_col_Alloc(vm);
        nc.col_start = col_pos;
        nc.col_wid = display_wid[c];
        nc.name_len = ch_N(col_name[c]);
        nc.col_name = col_name[c];
        tempstr qname;
        qname << field_base->ctype << "." << col_name[c];
        acr_nav::FField *fld = acr_nav::ind_field_Find(qname);
        bool navigable = false;
        if (fld) {
            navigable = live_mode
                ? FindPoolEntry(fld->p_arg->ctype)
                : (fld->p_reftype->up && FindSsimfile(*fld->p_arg));
        }
        if (navigable) {
            nc.target_ctype = fld->p_arg->ctype;
        }
        col_pos += display_wid[c];
    }
    vm.total_content_wid = col_pos;
}

// Find comment column index, build aligned header string from column names.
static void BuildPreviewHeader(acr_nav::FViewmode &vm, algo::cstring *col_name, int *display_wid, int n_col, int &comment_col) {
    comment_col = -1;
    for (int c = 0; c < n_col; c++) {
        if (algo::strptr_Eq(strptr(col_name[c]), "comment")) {
            comment_col = c;
            break;
        }
    }
    if (n_col > 0) {
        tempstr hdr;
        for (int c = 0; c < n_col; c++) {
            if (c > 0) {
                hdr << "  ";
            }
            hdr << col_name[c];
            char_PrintNTimes(' ', hdr, display_wid[c] - ch_N(col_name[c]));
        }
        vm.header = hdr;
    }
}

// Second-pass scan of mmap'd ssimfile: format data rows with aligned columns,
// add color spans for pkey and comment columns.
static void FormatPreviewRows(acr_nav::FViewmode &vm, algo_lib::MmapFile &file, int *display_wid, int n_col, int comment_col) {
    ind_beg(Line_curs, line, file.text) {
        algo::Tuple tuple;
        if (algo::Tuple_ReadStrptr(tuple, line, false)) {
            tempstr row;
            int col_byte_pos[64];
            FormatPreviewRow(row, tuple, display_wid, n_col, col_byte_pos);
            acr_nav::content_row_Alloc(vm).text = row;
            int li = acr_nav::content_row_N(vm) - 1;
            if (n_col > 0) {
                int pkey_end = (n_col > 1) ? col_byte_pos[1] - 2 : ch_N(row);
                AddSpan(vm, li, 0, pkey_end, acr_nav::ind_navstyle_Find("line_key"));
            }
            if (comment_col >= 0) {
                AddSpan(vm, li, col_byte_pos[comment_col], ch_N(row), acr_nav::ind_navstyle_Find("line_comment"));
            }
        }
    } ind_end;
}

static void LoadPreview(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("preview");
    tempstr pending(acr_nav::_db.preview_nav_pending);
    acr_nav::_db.preview_nav_pending = "";
    ClearViewmodeLines(vm);
    vm.header = "";
    vm.preview_h_scroll = 0;
    vm.cached_key = ctype.ctype;
    acr_nav::FSsimfile *ssimfile = FindSsimfile(ctype);
    if (ssimfile) {
        tempstr path;
        path << "data/" << ssimns_Get(*ssimfile) << "/"
             << name_Get(*ssimfile) << ".ssim";
        algo_lib::MmapFile file;
        if (algo_lib::MmapFile_Load(file, path)) {
            int n_col = 0;
            int display_wid[64];
            algo::cstring col_name[64];
            MeasurePreviewColumns(file, col_name, display_wid, n_col);
            DetectNavColumns(vm, ssimfile->p_ctype, col_name, display_wid, n_col, false);
            vm.pkey_wid = (n_col > 0) ? display_wid[0] : 0;
            int comment_col = -1;
            BuildPreviewHeader(vm, col_name, display_wid, n_col, comment_col);
            FormatPreviewRows(vm, file, display_wid, n_col, comment_col);
            // Apply deferred follow-ref match
            if (ch_N(pending) > 0 && vm.pkey_wid > 0) {
                int n_lines = acr_nav::content_row_N(vm);
                for (int i = 0; i < n_lines; i++) {
                    algo::strptr row = acr_nav::content_row_qFind(vm, i).text;
                    int end = i32_Min(DisplayToByte(row, vm.pkey_wid), elems_N(row));
                    algo::strptr pkey_raw(row.elems, end);
                    tempstr pkey;
                    pkey << algo::TrimmedRight(pkey_raw);
                    if (algo::strptr_Eq(strptr(pkey), algo::TrimmedRight(strptr(pending)))) {
                        acr_nav::_db.p_right_panel->sel_row = i;
                        break;
                    }
                }
            }
        }
    }
    if (acr_nav::_db.sel_nav_col_pending >= 0) {
        acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col_pending;
        acr_nav::_db.sel_nav_col_pending = -1;
    } else {
        acr_nav::_db.sel_nav_col = 0;
    }
}

// True if the identifier matches a C++ keyword commonly found in amc-generated output.
// Linear scan of ~26 entries; adequate for per-line highlighting.
static bool IsKw(algo::strptr word) {
    static const algo::strptr kw[] = {
        "bool", "const", "delete", "enum", "explicit", "extern",
        "friend", "inline", "namespace", "operator", "static",
        "struct", "template", "typename", "using", "void", "virtual",
        "__attribute__",
        "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"
    };
    bool ret = false;
    for (int i = 0; i < (int)(sizeof(kw)/sizeof(kw[0])); i++) {
        if (algo::strptr_Eq(word, kw[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}

// Tokenize one line of C++ code and emit highlight spans for keywords, strings, comments, and preprocessor directives.
// Single-pass left-to-right scan. Spans are emitted in col_start order (required by renderer).
// Handles: // comments (not inside strings), "..." strings (with \" escapes),
// #include/#pragma/#ifdef/#endif at line start, ~18 C++ keywords with word-boundary checks.
// Defers: /* */ multi-line comments, raw string literals (not in amc output).
static void HighlightCppLine(acr_nav::FViewmode &vm, int line_idx, algo::strptr line) {
    int len = line.n_elems;
    int pos = 0;
    // Check for preprocessor directive: # at start of line (after optional whitespace)
    int pp = 0;
    while (pp < len && (line.elems[pp] == ' ' || line.elems[pp] == '\t')) {
        pp++;
    }
    if (pp < len && line.elems[pp] == '#') {
        AddSpan(vm, line_idx, 0, len, acr_nav::ind_navstyle_Find("line_preproc"));
        pos = len; // done with this line
    }
    while (pos < len) {
        char c = line.elems[pos];
        // Check for // line comment
        bool is_comment = (c == '/' && pos + 1 < len && line.elems[pos + 1] == '/');
        if (is_comment) {
            AddSpan(vm, line_idx, pos, len, acr_nav::ind_navstyle_Find("line_comment"));
            pos = len; // done with this line
        }
        // Check for string literal
        if (pos < len && line.elems[pos] == '"') {
            int start = pos;
            pos++; // skip opening quote
            bool closed = false;
            while (pos < len && !closed) {
                char sc = line.elems[pos];
                if (sc == '\\' && pos + 1 < len) {
                    pos += 2; // skip escaped character
                } else if (sc == '"') {
                    pos++; // skip closing quote
                    closed = true;
                } else {
                    pos++;
                }
            }
            AddSpan(vm, line_idx, start, pos, acr_nav::ind_navstyle_Find("line_string"));
        }
        // Check for identifier (potential keyword)
        if (pos < len) {
            char ic = line.elems[pos];
            bool is_alpha = (ic >= 'A' && ic <= 'Z') || (ic >= 'a' && ic <= 'z') || ic == '_';
            if (is_alpha) {
                int start = pos;
                while (pos < len) {
                    char wc = line.elems[pos];
                    bool is_word = (wc >= 'A' && wc <= 'Z') || (wc >= 'a' && wc <= 'z')
                        || (wc >= '0' && wc <= '9') || wc == '_';
                    if (!is_word) {
                        break;
                    }
                    pos++;
                }
                // Word boundary check: char before start must be non-word or start of line
                bool before_ok = (start == 0);
                if (!before_ok) {
                    char bc = line.elems[start - 1];
                    before_ok = !((bc >= 'A' && bc <= 'Z') || (bc >= 'a' && bc <= 'z')
                        || (bc >= '0' && bc <= '9') || bc == '_');
                }
                // Char after end must be non-word or end of line
                bool after_ok = (pos == len);
                if (!after_ok) {
                    char ac = line.elems[pos];
                    after_ok = !((ac >= 'A' && ac <= 'Z') || (ac >= 'a' && ac <= 'z')
                        || (ac >= '0' && ac <= '9') || ac == '_');
                }
                if (before_ok && after_ok) {
                    algo::strptr word(line.elems + start, pos - start);
                    if (IsKw(word)) {
                        AddSpan(vm, line_idx, start, pos, acr_nav::ind_navstyle_Find("line_keyword"));
                    }
                }
            } else {
                pos++; // advance past non-interesting character
            }
        }
    }
}

// Load amc-generated C++ struct definition for a ctype into the codegen viewmode.
// Ctype names come from trusted ssimfile data loaded at startup.
static void LoadCodegen(acr_nav::FCtype &ctype) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("codegen");
    ClearViewmodeLines(vm);
    vm.cached_key = ctype.ctype;
    tempstr cmd;
    cmd << "amc '" << ctype.ctype << "'";
    vm.header = cmd;
    tempstr output = SysEval(cmd, FailokQ(true), 64*1024);
    ind_beg(Line_curs, line, output) {
        if (!StartsWithQ(line, "report.")) {
            acr_nav::content_row_Alloc(vm).text = line;
            HighlightCppLine(vm, acr_nav::content_row_N(vm) - 1, line);
        }
    } ind_end;
}

// Per-namespace dependency count for LoadNsDep accumulation.
struct NsDep { acr_nav::FNs *ns; int count; };

// Per-namespace field group for LoadNsDepDetail accumulation.
struct NsFieldGroup {
    acr_nav::FNs *ns;
    acr_nav::FField *fields[256]; // fixed capacity; silently truncates if exceeded
    int n_field;
};

// Format a sorted section of namespace dependency counts as highlighted text lines.
// Sorts deps[0..n) by count descending, emits a header line and one row per dep.
static void FormatNsDepSection(acr_nav::FViewmode &vm, algo::strptr header, NsDep *deps, int n) {
    // Sort by count descending (insertion sort)
    for (int i = 1; i < n; i++) {
        NsDep tmp = deps[i];
        int j = i - 1;
        while (j >= 0 && deps[j].count < tmp.count) {
            deps[j + 1] = deps[j];
            j--;
        }
        deps[j + 1] = tmp;
    }
    // Section header
    {
        tempstr hdr;
        hdr << header;
        acr_nav::content_row_Alloc(vm).text = hdr;
        AddSpan(vm, acr_nav::content_row_N(vm) - 1, 0, ch_N(hdr), acr_nav::ind_navstyle_Find("line_section"));
    }
    if (n == 0) {
        acr_nav::content_row_Alloc(vm).text = "  (none)";
    }
    // Find max ns name width for alignment
    int max_wid = 0;
    for (int i = 0; i < n; i++) {
        int name_wid = ch_N(NsDisplayName(*deps[i].ns));
        max_wid = i32_Max(max_wid, name_wid);
    }
    // Format each row
    for (int i = 0; i < n; i++) {
        algo::strptr name = NsDisplayName(*deps[i].ns);
        tempstr row;
        row << "  ";
        char_PrintNTimes(' ', row, max_wid - ch_N(name));
        int ns_start = ch_N(row);
        row << name;
        int ns_end = ch_N(row);
        row << "  ";
        int cnt = deps[i].count;
        int digs = DecimalDigits(cnt);
        char_PrintNTimes(' ', row, i32_Max(0, 5 - digs));
        row << cnt;
        row << (cnt == 1 ? " field" : " fields");
        acr_nav::content_row_Alloc(vm).text = row;
        AddSpan(vm, acr_nav::content_row_N(vm) - 1, ns_start, ns_end, acr_nav::ind_navstyle_Find("line_key"));
    }
}

// Extract namespace pointer from a nsdep viewmode line by finding the line_key
// color span (which highlights the namespace name) and looking up the namespace.
// Returns NULL for header, separator, or "(none)" lines.
acr_nav::FNs* acr_nav::NsDepNsAtLine(acr_nav::FViewmode &vm, int line_idx) {
    acr_nav::FNavstyle *line_key_style = acr_nav::ind_navstyle_Find("line_key");
    acr_nav::FNs *ret = NULL;
    if (line_idx >= 0 && line_idx < acr_nav::content_row_N(vm)) {
        algo::strptr line_text = acr_nav::content_row_qFind(vm, line_idx).text;
        bool found = false;
        for (int si = 0; si < acr_nav::cspan_N(vm) && !found; si++) {
            acr_nav::LineColorSpan &span = acr_nav::cspan_qFind(vm, si);
            if (span.line_idx == line_idx && span.p_navstyle == line_key_style) {
                int end = i32_Min(span.col_end, elems_N(line_text));
                if (span.col_start < end) {
                    algo::strptr name(line_text.elems + span.col_start, end - span.col_start);
                    ret = acr_nav::ind_ns_Find(name);
                    if (!ret && algo::strptr_Cmp(name, "other") == 0) {
                        ret = acr_nav::ind_ns_Find("");
                    }
                }
                found = true;
            }
        }
    }
    return ret;
}

// Find-or-insert namespace in accumulator array, increment count.
static void AccumNsDep(NsDep *deps, int &n, int max_n, acr_nav::FNs *ns) {
    bool found = false;
    for (int i = 0; i < n && !found; i++) {
        if (deps[i].ns == ns) {
            deps[i].count++;
            found = true;
        }
    }
    if (!found && n < max_n) {
        deps[n].ns = ns;
        deps[n].count = 1;
        n++;
    }
}

// Format one direction (upstream or downstream) of per-field namespace dependency detail.
// Sorts groups by field count descending, emits section header with totals,
// per-namespace sub-headers with field rows, or "(none)" if empty.
static void FormatNsDepDetailSection(acr_nav::FViewmode &vm, algo::strptr header, NsFieldGroup *groups, int n, int arg_col, int ref_col) {
    // Sort groups by field count descending (insertion sort)
    for (int i = 1; i < n; i++) {
        NsFieldGroup tmp = groups[i];
        int j = i - 1;
        while (j >= 0 && groups[j].n_field < tmp.n_field) {
            groups[j + 1] = groups[j];
            j--;
        }
        groups[j + 1] = tmp;
    }
    int total = 0;
    for (int gi = 0; gi < n; gi++) total += groups[gi].n_field;
    // Section header with totals
    {
        tempstr hdr;
        hdr << header
            << total << (total == 1 ? " field, " : " fields, ")
            << n << (n == 1 ? " namespace" : " namespaces");
        acr_nav::ContentRow &cr = acr_nav::content_row_Alloc(vm);
        cr.text = hdr;
        acr_nav::nav_target_Alloc(cr) = "";
        AddSpan(vm, acr_nav::content_row_N(vm) - 1, 0, ch_N(hdr), acr_nav::ind_navstyle_Find("line_section"));
    }
    for (int gi = 0; gi < n; gi++) {
        NsFieldGroup &g = groups[gi];
        tempstr section;
        section << G_HORIZ << G_HORIZ << " " << NsDisplayName(*g.ns)
                << " (" << g.n_field << (g.n_field == 1 ? " field) " : " fields) ");
        int display_width = ch_N(section) - Utf8ExtraBytes(strptr(section));
        int fill = i32_Max(0, 50 - display_width);
        for (int i = 0; i < fill; i++) section << G_HORIZ;
        acr_nav::ContentRow &scr = acr_nav::content_row_Alloc(vm);
        scr.text = section;
        acr_nav::nav_target_Alloc(scr) = "";
        AddSpan(vm, acr_nav::content_row_N(vm) - 1, 0, ch_N(section), acr_nav::ind_navstyle_Find("line_section"));
        for (int fi = 0; fi < g.n_field; fi++) {
            acr_nav::FField &fld = *g.fields[fi];
            tempstr row;
            row << "  " << fld.field;
            char_PrintNTimes(' ', row, arg_col - ch_N(row) + 2);
            int arg_start = ch_N(row);
            row << fld.p_arg->ctype;
            int arg_end = ch_N(row);
            char_PrintNTimes(' ', row, ref_col - ch_N(row) + 2);
            row << fld.reftype;
            acr_nav::ContentRow &cr = acr_nav::content_row_Alloc(vm);
            cr.text = row;
            acr_nav::nav_target_Alloc(cr) = fld.p_arg->ctype;
            int li = acr_nav::content_row_N(vm) - 1;
            AddSpan(vm, li, 2, 2 + ch_N(fld.field), acr_nav::ind_navstyle_Find("line_key"));
            AddSpan(vm, li, arg_start, arg_end, acr_nav::ind_navstyle_Find("line_comment"));
        }
    }
    if (n == 0) {
        acr_nav::ContentRow &ncr = acr_nav::content_row_Alloc(vm);
        ncr.text = "  (none)";
        acr_nav::nav_target_Alloc(ncr) = "";
    }
}

// Compute and display cross-namespace field dependencies for a given namespace.
// Upstream: namespaces this ns imports from (via field arg references).
// Downstream: namespaces that import from this ns (via field_arg back-references).
void acr_nav::LoadNsDep(acr_nav::FNs &ns) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("nsdep");
    ClearViewmodeLines(vm);
    acr_nav::_db.p_nsdep_ns = &ns;
    vm.cached_key = ns.ns;
    // Fixed-size accumulator for per-namespace counts
    NsDep deps[256];
    if (acr_nav::ns_N() > 256) {
        acr_nav::content_row_Alloc(vm).text = "(too many namespaces)";
    } else {
        // --- Upstream: fields in this ns whose arg is in another ns ---
        int n_up = 0;
        ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
            ind_beg(acr_nav::ctype_c_field_curs, fld, ct) {
                if (fld.p_arg && fld.p_arg->p_ns != &ns) {
                    AccumNsDep(deps, n_up, 256, fld.p_arg->p_ns);
                }
            } ind_end;
        } ind_end;
        algo::strptr display_name = NsDisplayName(ns);
        // Format upstream section
        {
            tempstr hdr;
            hdr << "Upstream (" << display_name << " imports from):";
            FormatNsDepSection(vm, hdr, deps, n_up);
        }
        // --- Downstream: fields from OTHER ns whose arg points to ctypes in this ns ---
        NsDep down[256];
        int n_down = 0;
        ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
            ind_beg(acr_nav::ctype_c_field_arg_curs, fld, ct) {
                if (fld.p_ctype->p_ns != &ns) {
                    AccumNsDep(down, n_down, 256, fld.p_ctype->p_ns);
                }
            } ind_end;
        } ind_end;
        // Blank separator
        acr_nav::content_row_Alloc(vm).text = "";
        // Format downstream section
        {
            tempstr hdr;
            hdr << "Downstream (imports from " << display_name << "):";
            FormatNsDepSection(vm, hdr, down, n_down);
        }
    }
    vm.header = NsDisplayName(ns);
}

// Ensure-content wrappers for hook dispatch.
// Each normalizes the lazy-load check to the ensure_content hook signature.
void acr_nav::viewmode_preview_ensure_content(acr_nav::FCtype &ct) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("preview");
    if (vm.cached_key != ct.ctype) {
        LoadPreview(ct);
    }
}

void acr_nav::viewmode_codegen_ensure_content(acr_nav::FCtype &ct) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("codegen");
    if (vm.cached_key != ct.ctype) {
        LoadCodegen(ct);
    }
}

void acr_nav::viewmode_nsdep_ensure_content(acr_nav::FCtype &ct) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("nsdep");
    if (vm.cached_key != ct.p_ns->ns) {
        LoadNsDep(*ct.p_ns);
    }
}

void acr_nav::viewmode_fields_ensure_content(acr_nav::FCtype &) {
}
void acr_nav::viewmode_xref_ensure_content(acr_nav::FCtype &) {
}
void acr_nav::viewmode_help_ensure_content(acr_nav::FCtype &) {
}
void acr_nav::viewmode_detail_ensure_content(acr_nav::FCtype &) {
}

// Populate inspect viewmode with columnar-formatted records from the live state dump.
// Two-pass pipeline: measure column widths, then format aligned rows.
// Census/index lines appended as unformatted comments after records.
void acr_nav::viewmode_inspect_ensure_content(acr_nav::FCtype &ct) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("inspect");
    tempstr gen_key;
    gen_key << "inspect:" << acr_nav::_db.live_generation << ":" << ct.ctype;
    if (vm.cached_key != gen_key) {
        tempstr pending(acr_nav::_db.preview_nav_pending);
        acr_nav::_db.preview_nav_pending = "";
        ClearViewmodeLines(vm);
        vm.header = "";
        vm.preview_h_scroll = 0;
        vm.cached_key = gen_key;
        if (!acr_nav::_db.live_connected) {
            tempstr msg;
            if (ch_N(acr_nav::_db.live_error) > 0) {
                msg << acr_nav::_db.live_error;
            } else {
                msg << "not connected (use -connect <socket>)";
            }
            acr_nav::content_row_Alloc(vm).text = msg;
        } else if (acr_nav::_db.live_generation == 0) {
            acr_nav::content_row_Alloc(vm).text = "waiting for first poll response...";
        } else {
            tempstr prefix;
            prefix << ct.ctype << "  ";
            tempstr census_match;
            census_match << "ctype:" << ct.ctype;
            // Pass 1: measure column widths from record lines
            int n_col = 0;
            int display_wid[64];
            algo::cstring col_name[64];
            int n_records = 0;
            int n_census = 0;
            ind_beg(Line_curs, line, acr_nav::_db.live_data) {
                bool is_record = algo::StartsWithQ(line, strptr(prefix));
                if (is_record) {
                    algo::Tuple tuple;
                    if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                        MeasureTupleColumns(tuple, col_name, display_wid, n_col);
                        n_records++;
                    }
                } else {
                    bool is_census = algo::StartsWithQ(line, strptr("report.PoolCensus"))
                        && algo::FindStr(line, strptr(census_match)) >= 0;
                    bool is_idx = algo::StartsWithQ(line, strptr("report.IndexCensus"))
                        && algo::FindStr(line, strptr(census_match)) >= 0;
                    if (is_census || is_idx) {
                        n_census++;
                    }
                }
            } ind_end;
            // Detect nav columns and build header
            if (n_records > 0) {
                DetectNavColumns(vm, &ct, col_name, display_wid, n_col, true);
                vm.pkey_wid = (n_col > 0) ? display_wid[0] : 0;
                int comment_col = -1;
                BuildPreviewHeader(vm, col_name, display_wid, n_col, comment_col);
                // Pass 2: format record rows with aligned columns
                ind_beg(Line_curs, line, acr_nav::_db.live_data) {
                    if (algo::StartsWithQ(line, strptr(prefix))) {
                        algo::Tuple tuple;
                        if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                            tempstr row;
                            int col_byte_pos[64];
                            FormatPreviewRow(row, tuple, display_wid, n_col, col_byte_pos);
                            acr_nav::content_row_Alloc(vm).text = row;
                            int li = acr_nav::content_row_N(vm) - 1;
                            if (n_col > 0) {
                                int pkey_end = (n_col > 1) ? col_byte_pos[1] - 2 : ch_N(row);
                                AddSpan(vm, li, 0, pkey_end, acr_nav::ind_navstyle_Find("line_key"));
                            }
                            if (comment_col >= 0) {
                                AddSpan(vm, li, col_byte_pos[comment_col], ch_N(row), acr_nav::ind_navstyle_Find("line_comment"));
                            }
                        }
                    }
                } ind_end;
            }
            // Append census/index lines as unformatted comments
            ind_beg(Line_curs, line, acr_nav::_db.live_data) {
                bool is_census = algo::StartsWithQ(line, strptr("report.PoolCensus"))
                    && algo::FindStr(line, strptr(census_match)) >= 0;
                bool is_idx = algo::StartsWithQ(line, strptr("report.IndexCensus"))
                    && algo::FindStr(line, strptr(census_match)) >= 0;
                if (is_census || is_idx) {
                    acr_nav::content_row_Alloc(vm).text = line;
                    int li = acr_nav::content_row_N(vm) - 1;
                    AddSpan(vm, li, 0, elems_N(line), acr_nav::ind_navstyle_Find("line_comment"));
                }
            } ind_end;
            if (n_records == 0 && n_census == 0) {
                acr_nav::content_row_Alloc(vm).text = "no records in dump for this type";
            }
            // Apply deferred follow-ref match
            if (ch_N(pending) > 0 && vm.pkey_wid > 0) {
                int n_lines = acr_nav::content_row_N(vm);
                for (int i = 0; i < n_lines; i++) {
                    algo::strptr row = acr_nav::content_row_qFind(vm, i).text;
                    int end = i32_Min(DisplayToByte(row, vm.pkey_wid), elems_N(row));
                    algo::strptr pkey_raw(row.elems, end);
                    tempstr pkey;
                    pkey << algo::TrimmedRight(pkey_raw);
                    if (algo::strptr_Eq(strptr(pkey), algo::TrimmedRight(strptr(pending)))) {
                        acr_nav::_db.p_right_panel->sel_row = i;
                        break;
                    }
                }
            }
        }
    }
    if (acr_nav::_db.sel_nav_col_pending >= 0) {
        acr_nav::_db.sel_nav_col = acr_nav::_db.sel_nav_col_pending;
        acr_nav::_db.sel_nav_col_pending = -1;
    } else if (acr_nav::nav_col_N(vm) > 0 && acr_nav::_db.sel_nav_col >= acr_nav::nav_col_N(vm)) {
        acr_nav::_db.sel_nav_col = 0;
    }
}

// Add a field to the namespace field group matching 'ns'.
// Creates a new group if none exists and capacity allows.
static void AddNsFieldGroup(NsFieldGroup *groups, int &n_group, int max_groups,
                            acr_nav::FNs *ns, acr_nav::FField &field) {
    int gi = -1;
    for (int i = 0; i < n_group; i++) {
        if (groups[i].ns == ns) { gi = i; break; }
    }
    if (gi < 0 && n_group < max_groups) {
        gi = n_group++;
        groups[gi].ns = ns;
        groups[gi].n_field = 0;
    }
    if (gi >= 0 && groups[gi].n_field < 256) {  // NsFieldGroup.fields[] capacity
        groups[gi].fields[groups[gi].n_field++] = &field;
    }
}

// Per-field cross-namespace dependency detail.
// Groups fields by foreign namespace, showing the actual field→arg references.
static void LoadNsDepDetail(acr_nav::FNs &ns) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("nsdep_detail");
    ClearViewmodeLines(vm);
    vm.cached_key = ns.ns;
    acr_nav::_db.p_nsdep_ns = &ns;
    algo::strptr display_name = NsDisplayName(ns);
    NsFieldGroup up_groups[64];
    int n_up = 0;
    NsFieldGroup down_groups[64];
    int n_down = 0;
    // Upstream: fields in this ns whose arg is in another ns
    ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
        ind_beg(acr_nav::ctype_c_field_curs, fld, ct) {
            if (fld.p_arg && fld.p_arg->p_ns != &ns) {
                AddNsFieldGroup(up_groups, n_up, 64, fld.p_arg->p_ns, fld);
            }
        } ind_end;
    } ind_end;
    // Downstream: fields from other ns whose arg is a ctype in this ns
    ind_beg(acr_nav::ns_c_ctype_curs, ct, ns) {
        ind_beg(acr_nav::ctype_c_field_arg_curs, fld, ct) {
            if (fld.p_ctype->p_ns != &ns) {
                AddNsFieldGroup(down_groups, n_down, 64, fld.p_ctype->p_ns, fld);
            }
        } ind_end;
    } ind_end;
    // Measure column widths across both directions
    int max_field_wid = 5;  // "field" header
    int max_arg_wid = 3;    // "arg" header
    int max_ref_wid = 7;    // "reftype" header
    for (int d = 0; d < 2; d++) {
        NsFieldGroup *groups = d == 0 ? up_groups : down_groups;
        int n = d == 0 ? n_up : n_down;
        for (int gi = 0; gi < n; gi++) {
            for (int fi = 0; fi < groups[gi].n_field; fi++) {
                acr_nav::FField &fld = *groups[gi].fields[fi];
                max_field_wid = i32_Max(max_field_wid, ch_N(fld.field));
                max_arg_wid = i32_Max(max_arg_wid, ch_N(fld.p_arg->ctype));
                max_ref_wid = i32_Max(max_ref_wid, ch_N(fld.reftype));
            }
        }
    }
    int arg_col = max_field_wid + 2;
    int ref_col = arg_col + max_arg_wid + 2;
    acr_nav::PreviewNavCol &nc = acr_nav::nav_col_Alloc(vm);
    nc.col_start = arg_col;
    nc.col_wid = max_arg_wid;
    nc.col_name = "arg";
    nc.target_ctype = "";
    // Format upstream and downstream sections
    {
        tempstr up_hdr;
        up_hdr << "Upstream (" << display_name << " imports from): ";
        FormatNsDepDetailSection(vm, up_hdr, up_groups, n_up, arg_col, ref_col);
    }
    {
        acr_nav::ContentRow &sep = acr_nav::content_row_Alloc(vm);
        sep.text = "";
        acr_nav::nav_target_Alloc(sep) = "";
    }
    {
        tempstr down_hdr;
        down_hdr << "Downstream (imports from " << display_name << "): ";
        FormatNsDepDetailSection(vm, down_hdr, down_groups, n_down, arg_col, ref_col);
    }
    vm.header = NsDisplayName(ns);
}

void acr_nav::viewmode_nsdep_detail_ensure_content(acr_nav::FCtype &ct) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("nsdep_detail");
    if (vm.cached_key != ct.p_ns->ns) {
        LoadNsDepDetail(*ct.p_ns);
    }
}

// Emit a section header line: "── title ──────..." with line_section highlight.
static void EmitSectionHeader(acr_nav::FViewmode &vm, algo::strptr title) {
    tempstr hdr;
    hdr << G_HORIZ << G_HORIZ << " " << title << " ";
    int display_width = ch_N(hdr) - Utf8ExtraBytes(strptr(hdr));
    int fill = i32_Max(0, 36 - display_width);
    for (int i = 0; i < fill; i++) hdr << G_HORIZ;
    acr_nav::content_row_Alloc(vm).text = hdr;
    AddSpan(vm, acr_nav::content_row_N(vm) - 1, 0, ch_N(hdr), acr_nav::ind_navstyle_Find("line_section"));
}

// Format a single ssim record as a vertical card: section header + one key:value per line.
// Appends formatted lines to vm.line_elems. Skips primary key attr when its value
// matches field_name (already shown in the detail header bar).
static void FormatDetailCard(acr_nav::FViewmode &vm, algo::Tuple &tuple, algo::strptr field_name) {
    // Compute key column width (max attr name length, excluding redundant pkey)
    int key_wid = 0;
    int ai = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        bool is_redundant_pkey = (ai == 0) && (attr.value == field_name);
        if (!is_redundant_pkey) {
            key_wid = i32_Max(key_wid, ch_N(attr.name));
        }
        ai++;
    } ind_end;
    tempstr head_str;
    head_str << tuple.head;
    EmitSectionHeader(vm, strptr(head_str));
    // Key:value rows
    ai = 0;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        bool is_redundant_pkey = (ai == 0) && (attr.value == field_name);
        if (!is_redundant_pkey) {
            tempstr row;
            row << "  " << attr.name;
            char_PrintNTimes(' ', row, i32_Max(2, key_wid - ch_N(attr.name) + 4));
            tempstr safe(attr.value);
            SanitizeForDisplay(safe);
            row << safe;
            acr_nav::content_row_Alloc(vm).text = row;
            AddSpan(vm, acr_nav::content_row_N(vm) - 1, 2, 2 + ch_N(attr.name), acr_nav::ind_navstyle_Find("line_key"));
        }
        ai++;
    } ind_end;
    acr_nav::content_row_Alloc(vm).text = strptr();
}

// Load metadata records for a single field from detailsrc ssimfiles.
// Re-serializes the dmmeta.field record as the first card, then scans each
// detailsrc file for matching records (first attribute value == field name).
void acr_nav::LoadDetail(acr_nav::FField &field) {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("detail");
    ClearViewmodeLines(vm);
    acr_nav::_db.p_detail_field = &field;
    algo::strptr field_name(field.field);
    int n_records = 0;
    // First card: the dmmeta.field record itself
    {
        dmmeta::Field base;
        acr_nav::field_CopyOut(field, base);
        tempstr fld_line;
        dmmeta::Field_Print(base, fld_line);
        algo::Tuple fld_tuple;
        if (algo::Tuple_ReadStrptr(fld_tuple, fld_line, false)) {
            FormatDetailCard(vm, fld_tuple, field_name);
            n_records++;
        }
    }
    // Second card: the dmmeta.reftype record for this field's reftype
    if (ch_N(field.p_reftype->comment) > 0) {
        dmmeta::Reftype base;
        acr_nav::reftype_CopyOut(*field.p_reftype, base);
        tempstr rt_line;
        dmmeta::Reftype_Print(base, rt_line);
        algo::Tuple rt_tuple;
        if (algo::Tuple_ReadStrptr(rt_tuple, rt_line, false)) {
            FormatDetailCard(vm, rt_tuple, field_name);
            n_records++;
        }
    }
    // Scan each detailsrc file for matching records.
    // Path derived via Pathcomp rather than Ssimfile accessors: detailsrc uses
    // a Smallstr50 key instead of a Pkey to dmmeta.Ssimfile, avoiding ~5 schema
    // records (finput, Upptr, xref) for display-only file scanning.
    ind_beg(acr_nav::_db_detailsrc_curs, ds, acr_nav::_db) {
        algo::strptr dskey(ds.detailsrc);
        algo::strptr ns = algo::Pathcomp(dskey, ".LL");
        algo::strptr name = algo::Pathcomp(dskey, ".LR");
        tempstr path;
        path << "data/" << ns << "/" << name << ".ssim";
        algo_lib::MmapFile file;
        if (algo_lib::MmapFile_Load(file, path)) {
            ind_beg(Line_curs, line, file.text) {
                algo::Tuple tuple;
                if (algo::Tuple_ReadStrptr(tuple, line, false)) {
                    if (attrs_N(tuple) > 0 && attrs_qFind(tuple, 0).value == field_name) {
                        FormatDetailCard(vm, tuple, field_name);
                        n_records++;
                    }
                }
            } ind_end;
        }
    } ind_end;
    // Set header
    tempstr hdr;
    hdr << field.field << " (" << n_records << " records)";
    vm.header = hdr;
}

int acr_nav::RightPanelItemCount(acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (!acr_nav::_db.p_cur_viewmode->has_fields) {
        if (sel_ct && acr_nav::_db.p_cur_viewmode->ensure_content) {
            acr_nav::ensure_content_Call(*acr_nav::_db.p_cur_viewmode, *sel_ct);
        }
        // Namespace-scoped viewmodes: when sel_ct is NULL (namespace header),
        // trigger ensure_content via a proxy ctype from the selected namespace.
        if (!sel_ct && acr_nav::_db.p_cur_viewmode->scope_ns) {
            acr_nav::FNs *ns = SelectedNs();
            if (ns && acr_nav::c_ctype_N(*ns) > 0) {
                acr_nav::FCtype *proxy = acr_nav::c_ctype_Find(*ns, 0);
                if (proxy) {
                    acr_nav::ensure_content_Call(*acr_nav::_db.p_cur_viewmode, *proxy);
                }
            }
        }
        ret = RightPanelLineCount();
    } else if (sel_ct) {
        bool reverse = acr_nav::_db.p_cur_viewmode->is_reverse;
        ret = reverse ? c_field_arg_N(*sel_ct) : c_field_N(*sel_ct);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Panel item count dispatches on position (0=ctype list, 1=field list).
// The two panels have structurally different roles (parent/child),
// not N instances of one concept -- factoring into step hooks
// would move the if elsewhere without reducing complexity.
int acr_nav::PanelItemCount(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (panel.position == 0) {
        ret = acr_nav::left_item_N();
    } else if (panel.position == 1) {
        ret = RightPanelItemCount(sel_ct);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Append key display name, mapping arrow keys to Unicode symbols.
static void AppendKeyDisplay(cstring &out, algo::strptr key) {
    struct { const char *key; const char *symbol; } arrows[] = {
        {"Up",    "\xe2\x86\x91"},
        {"Down",  "\xe2\x86\x93"},
        {"Left",  "\xe2\x86\x90"},
        {"Right", "\xe2\x86\x92"},
    };
    bool mapped = false;
    for (int i = 0; i < (int)(sizeof(arrows)/sizeof(arrows[0])); i++) {
        if (key == algo::strptr(arrows[i].key)) {
            out << arrows[i].symbol;
            mapped = true;
            break;
        }
    }
    if (!mapped) {
        out << key;
    }
}

// Collect browse-mode keybinds for one action into a single key string.
// Standard keys (arrows, Enter, etc.) come first, then letter aliases.
static void CollectActionKeys(acr_nav::FNavaction *action, cstring &keys) {
    // Two passes: first non-letter keys, then letter keys
    for (int pass = 0; pass < 2; pass++) {
        ind_beg(acr_nav::_db_keybind_curs, kb, acr_nav::_db) {
            if (kb.p_navaction == action && acr_nav::navmode_Get(kb) == "browse") {
                algo::Smallstr50 key = acr_nav::key_Get(kb);
                algo::strptr keystr(key);
                bool is_letter = (elems_N(keystr) == 1 && ((keystr[0] >= 'a' && keystr[0] <= 'z') || (keystr[0] >= 'A' && keystr[0] <= 'Z')));
                if ((pass == 0 && !is_letter) || (pass == 1 && is_letter)) {
                    if (ch_N(keys) > 0) {
                        keys << "/";
                    }
                    AppendKeyDisplay(keys, keystr);
                }
            }
        } ind_end;
    }
}

// Check if two consecutive navactions form a directional pair (up/down, left/right, top/bottom).
static bool IsDirPair(acr_nav::FNavaction *a, acr_nav::FNavaction *b) {
    bool ret = false;
    algo::strptr na(a->navaction);
    algo::strptr nb(b->navaction);
    int prefix = 0;
    while (prefix < na.n_elems && prefix < nb.n_elems && na.elems[prefix] == nb.elems[prefix]) {
        prefix++;
    }
    if (prefix > 0 && prefix < na.n_elems && prefix < nb.n_elems) {
        algo::strptr sa(na.elems + prefix, na.n_elems - prefix);
        algo::strptr sb(nb.elems + prefix, nb.n_elems - prefix);
        ret = (sa == "up" && sb == "down")
            || (sa == "left" && sb == "right")
            || (sa == "top" && sb == "bottom");
    }
    return ret;
}

// Merge comments of paired actions: combine first differing word with "/".
// "Move selection up" + "Move selection down" → "Move selection up/down"
static tempstr MergePairComments(algo::strptr c1, algo::strptr c2) {
    // Extract words from both comments
    tempstr words1[16], words2[16];
    int nw1 = 0, nw2 = 0;
    {
        int i = 0;
        while (i < c1.n_elems && nw1 < 16) {
            while (i < c1.n_elems && c1.elems[i] == ' ') i++;
            int start = i;
            while (i < c1.n_elems && c1.elems[i] != ' ') i++;
            if (i > start) { words1[nw1] << algo::strptr(c1.elems + start, i - start); nw1++; }
        }
    }
    {
        int i = 0;
        while (i < c2.n_elems && nw2 < 16) {
            while (i < c2.n_elems && c2.elems[i] == ' ') i++;
            int start = i;
            while (i < c2.n_elems && c2.elems[i] != ' ') i++;
            if (i > start) { words2[nw2] << algo::strptr(c2.elems + start, i - start); nw2++; }
        }
    }
    // Walk words, combine first differing pair with "/"
    tempstr result;
    int n = i32_Max(nw1, nw2);
    for (int w = 0; w < n; w++) {
        if (w > 0) { result << " "; }
        algo::strptr s1 = w < nw1 ? strptr(words1[w]) : strptr();
        algo::strptr s2 = w < nw2 ? strptr(words2[w]) : strptr();
        if (s1 == s2) {
            result << s1;
        } else if (s1.n_elems > 0 && s2.n_elems > 0) {
            result << s1 << "/" << s2;
        } else if (s1.n_elems > 0) {
            result << s1;
        } else {
            result << s2;
        }
    }
    return result;
}

// Build preformatted help lines from keybind/navaction data.
// Single-column layout with section headers styled like the detail view.
// Directional pairs (up/down, left/right) are merged into single lines.
// Arrow keys display as Unicode symbols (↑↓←→).
void acr_nav::BuildHelpLines() {
    acr_nav::FViewmode &vm = *acr_nav::ind_viewmode_Find("help");
    ClearViewmodeLines(vm);
    vm.header = "Keyboard Shortcuts";
    // Collect helpgroups sorted by sort_order
    acr_nav::FHelpgroup *groups[16]; // fixed capacity; silently truncates if exceeded
    int n_groups = 0;
    ind_beg(acr_nav::_db_helpgroup_curs, hg, acr_nav::_db) {
        if (n_groups < 16) {
            groups[n_groups++] = &hg;
        }
    } ind_end;
    for (int i = 1; i < n_groups; i++) {
        acr_nav::FHelpgroup *tmp = groups[i];
        int j = i;
        while (j > 0 && groups[j - 1]->sort_order > tmp->sort_order) {
            groups[j] = groups[j - 1];
            j--;
        }
        groups[j] = tmp;
    }
    // For each group, emit section header + action lines
    for (int g = 0; g < n_groups; g++) {
        EmitSectionHeader(vm, groups[g]->comment);
        // Collect navactions in this group, sorted by sort_order
        acr_nav::FNavaction *actions[32]; // fixed capacity; silently truncates if exceeded
        int n_actions = 0;
        ind_beg(acr_nav::_db_navaction_curs, na, acr_nav::_db) {
            if (na.p_helpgroup == groups[g] && n_actions < 32) {
                actions[n_actions++] = &na;
            }
        } ind_end;
        for (int i = 1; i < n_actions; i++) {
            acr_nav::FNavaction *tmp = actions[i];
            int j = i;
            while (j > 0 && actions[j - 1]->sort_order > tmp->sort_order) {
                actions[j] = actions[j - 1];
                j--;
            }
            actions[j] = tmp;
        }
        // Build a line for each navaction, merging directional pairs
        for (int a = 0; a < n_actions; a++) {
            bool is_pair = (a + 1 < n_actions) && IsDirPair(actions[a], actions[a + 1]);
            // Collect keys into single string
            tempstr keys;
            CollectActionKeys(actions[a], keys);
            if (is_pair) {
                CollectActionKeys(actions[a + 1], keys);
            }
            // Build comment
            tempstr comment;
            if (is_pair) {
                comment = MergePairComments(strptr(actions[a]->comment), strptr(actions[a + 1]->comment));
                a++;
            } else {
                comment << actions[a]->comment;
            }
            // Build line: "  keys          comment"
            tempstr line;
            line << "  " << keys;
            int keys_end = ch_N(line);
            int extra = Utf8ExtraBytes(strptr(keys));
            char_PrintNTimes(' ', line, i32_Max(2, 22 - ch_N(line) + extra));
            int comment_start = ch_N(line);
            line << comment;
            acr_nav::content_row_Alloc(vm).text = line;
            int li = acr_nav::content_row_N(vm) - 1;
            AddSpan(vm, li, 2, keys_end, acr_nav::ind_navstyle_Find("line_key"));
            AddSpan(vm, li, comment_start, ch_N(line), acr_nav::ind_navstyle_Find("line_comment"));
        }
    }
}
