---
name: statedump
description: Inspect acr_nav runtime state via IPC socket. Use when debugging acr_nav behavior, checking UI state, or investigating what the running process is doing.
user_invocable: true
---

# Statedump — acr_nav Runtime Inspection

Query the running acr_nav process through its IPC socket to inspect runtime state.

## Quick Reference

```bash
# Find the socket
ls /tmp/acr_nav.*.sock

# Full dump (all tables + census)
printf 'acr_nav.RequestStateDump filter:"%%"\n' | socat -t 5 - UNIX-CONNECT:/tmp/acr_nav.<pid>.sock

# Filter by ctype (SQL wildcards: % = any, _ = single char)
printf 'acr_nav.RequestStateDump filter:"acr_nav.FPanel"\n' | socat -t 5 - UNIX-CONNECT:/tmp/acr_nav.<pid>.sock

# Strip census lines (just records)
printf 'acr_nav.RequestStateDump filter:"%%"\n' | socat -t 5 - UNIX-CONNECT:/tmp/acr_nav.<pid>.sock | grep -v '^report\.'
```

## How to Use This Skill

When the user says `/statedump` or asks to inspect acr_nav state:

### Step 1: Find the socket

```bash
ls /tmp/acr_nav.*.sock
```

If multiple sockets exist, the stale one (from a dead process) won't respond. Pick the newest, or test with a quick query.

### Step 2: Query based on what the user needs

**Runtime overview** — the FDb singleton has all the interesting UI state (current panel, viewmode, filter, navstack pointers):

```bash
printf 'acr_nav.RequestStateDump filter:"acr_nav.FDb"\n' | socat -t 5 - UNIX-CONNECT:<socket>
```

**Specific table** — filter selects which ctypes to dump (not row-level grep). Use the exact ctype name:

```bash
printf 'acr_nav.RequestStateDump filter:"acr_nav.FPanel"\n' | socat -t 5 - UNIX-CONNECT:<socket>
```

**Multiple tables** — use SQL wildcard to match several:

```bash
# All types starting with "F" followed by single char + more
printf 'acr_nav.RequestStateDump filter:"acr_nav.FN%%"\n' | socat -t 5 - UNIX-CONNECT:<socket>
```

**Runtime-only state** (skip the 92% static schema data):

```bash
# The interesting runtime ctypes
for ctype in acr_nav.FDb acr_nav.FPanel acr_nav.LeftItem acr_nav.Naventry acr_nav.OverlayEntry acr_nav.FViewmode acr_nav.FNavmode; do
  printf "acr_nav.RequestStateDump filter:\"$ctype\"\n" | socat -t 5 - UNIX-CONNECT:<socket>
done | grep -v '^report\.'
```

**Census only** (pool sizes, no records — shows what's loaded):

```bash
printf 'acr_nav.RequestStateDump filter:"NOMATCH"\n' | socat -t 5 - UNIX-CONNECT:<socket>
```

### Step 3: Reconstruct the screen from statedump

The statedump contains all the raw data that headless mode reads from to build its screen capture. Here's how to reconstruct what the user sees.

#### Minimum queries needed (3 parallel socat calls):

```bash
SOCK=/tmp/acr_nav.<pid>.sock
# 1. UI state: mode, viewmode, filter, focus, pointers
printf 'acr_nav.RequestStateDump filter:"acr_nav.FDb"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
# 2. Panel cursor/scroll positions
printf 'acr_nav.RequestStateDump filter:"acr_nav.FPanel"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
# 3. Left panel rows (pre-built array, same one the renderer uses)
printf 'acr_nav.RequestStateDump filter:"acr_nav.LeftItem"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
```

Add these as needed:
```bash
# Nav stack (drill-in history, breadcrumb source)
printf 'acr_nav.RequestStateDump filter:"acr_nav.Naventry"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
# Overlay stack (help, detail overlays)
printf 'acr_nav.RequestStateDump filter:"acr_nav.OverlayEntry"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
# Fields of currently selected ctype (for right panel in fields viewmode)
printf 'acr_nav.RequestStateDump filter:"acr_nav.FField"\n' | socat -t 5 - UNIX-CONNECT:$SOCK
```

#### How to read the results:

**From FDb** (single record):
- `p_cur_panel` — which panel has focus ("ctype_list" = left, "content" = right)
- `p_cur_mode` — "browse" or "filter"
- `p_cur_viewmode` — what the right panel shows ("fields", "detail", "preview", "help", "source", etc.)
- `filter` — active filter text (empty = no filter)
- `p_cur_filtertarget` — what the filter searches ("ctype", "field", "comment", etc.)
- `sel_nav_col` — selected navigable column in right panel
- `p_detail_field` — field being inspected in detail view
- `p_nsdep_ns` — namespace for dependency views
- `term_hei` / `term_wid` — terminal dimensions

**From FPanel** (2 records: "ctype_list" and "content"):
- `sel_row` — cursor position (0-indexed row in that panel's data)
- `scroll_offset` — first visible row (viewport top)

**From LeftItem** (N records, one per visible left-panel row):
- Items are in display order — index directly with `ctype_list.sel_row`
- Namespace rows: `ctype:""` with `ns:<name>` (header for a collapsed/expanded group)
- Ctype rows: `ctype:<name>` with `ns:""` (actual type entry)
- `n_record` — pool census count for that ctype's ssimfile

**Deriving the selected ctype**: `LeftItem[FPanel("ctype_list").sel_row].ctype`

#### Presenting as a screen visualization:

Format the reconstruction as a text diagram showing both panels:

```
Mode:       <p_cur_mode>
Viewmode:   <p_cur_viewmode>
Focus:      <p_cur_panel> panel
Filter:     <filter text or "(none)">
Terminal:   <term_wid> x <term_hei>

Left panel (sel_row:<N>, scroll:<N>):

 0  > ns_name          [n_match]     <- collapsed namespace (> arrow)
 1  v ns_name          [n_match]     <- expanded namespace (v arrow)
 2    ctype.Name                     <- ctype under expanded ns
 3  » ctype.Name                     <- CURSOR (» marker)
 :    ...

Right panel — <viewmode title> of <selected ctype> (sel_row:<N>, scroll:<N>):

 field                           arg              reftype
 ─────────────────────────────────────────────────────────
 ns.Type.field1                  arg.Type         Reftype
 ns.Type.field2                  algo.Comment     Val
 :    ...

Nav stack: <empty | entries showing drill-in history>
Overlay:   <empty | active overlay viewmode>
```

### Known gaps (statedump vs headless)

| Data | In statedump? | Notes |
|------|---------------|-------|
| Left panel rows (LeftItem) | Yes | Pre-built array, exact display order |
| Cursor/scroll per panel | Yes | FPanel.sel_row, FPanel.scroll_offset |
| Viewmode, navmode, filter | Yes | FDb fields |
| Nav stack, overlay stack | Yes | Naventry, OverlayEntry pools |
| Namespace collapse state | Yes | FNs.collapsed, FNs.n_match |
| All field/ctype metadata | Yes | FField, FCtype pools |
| Right-panel rendered text | **No** | Computed on-the-fly by viewmode renderers |
| Breadcrumb string | **No** | Computed from nav stack, not stored |
| Status bar hints | **No** | Computed from navmode, not stored |
| Viewport row count | **No** | Requires DataRows() calculation |

To close these gaps, a `RequestScreenCapture` IPC command would call the existing `HeadlessOutput()` logic over the socket.

## Protocol Details

- **Message format**: `acr_nav.RequestStateDump filter:"<sql_pattern>"\n`
- **Filter scope**: matches against ctype names (e.g. `acr_nav.FPanel`), not individual record values
- **Filter syntax**: SQL wildcards (`%` = any chars, `_` = single char), NOT regex
- **Census lines**: always included regardless of filter — `report.PoolCensus` and `report.IndexCensus`
- **Response terminator**: empty line (`\n`)
- **Tool required**: `socat` (installed in devcontainer)
