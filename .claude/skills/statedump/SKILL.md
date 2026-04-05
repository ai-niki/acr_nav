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

### Step 3: Present results

- Lead with FDb fields — they show current panel, viewmode, filter, mode at a glance
- For large results, summarize rather than dumping raw output
- Cross-reference with schema: `acr dmmeta.ctype:acr_nav.%` to understand fields

## Protocol Details

- **Message format**: `acr_nav.RequestStateDump filter:"<sql_pattern>"\n`
- **Filter scope**: matches against ctype names (e.g. `acr_nav.FPanel`), not individual record values
- **Filter syntax**: SQL wildcards (`%` = any chars, `_` = single char), NOT regex
- **Census lines**: always included regardless of filter — `report.PoolCensus` and `report.IndexCensus`
- **Response terminator**: empty line (`\n`)
- **Tool required**: `socat` (installed in devcontainer)
