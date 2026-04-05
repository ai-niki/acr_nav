# Statedump Exploratory Test Report (2026-04-05)

## Summary

3 parallel subagents tested the statedump feature across batch dump, headless RequestStateDump, and IPC socket interfaces. **28 tests total, 1 bug found, 0 regressions.**

## Bug Found

### Bug 1: `-dump:""` bypasses StateDump entirely

**Severity:** Low (edge case)

**Description:** When `-dump:""` is passed, `ch_N(_db.cmdline.dump)` returns 0, so `do_dump` is false. The code falls through to HeadlessMain instead of calling StateDump, producing display-state lines (Screen, PanelState, VisibleLeftItem) with zero census lines. The headless `RequestStateDump filter:""` works correctly — empty filter matches nothing, produces census-only output.

**Reproduction:** `acr_nav -dump:"" 2>/dev/null | head -5`

**Fix:** Check `_db.cmdline.dump_isempty` or a dedicated bool instead of string length.

**Location:** `cpp/acr_nav/main.cpp` line ~1125: `bool do_dump = ch_N(_db.cmdline.dump) > 0;`

## Verified Working

### State Mutation + Cross-Validation (8 tests)
- FDb Ptr fields correctly populated in headless (p_cur_panel:ctype_list, p_cur_viewmode:help, etc.)
- Navigate changes p_cur_viewmode from help→fields, dismisses help overlay (OverlayEntry 1→0)
- Navstack records (Naventry pool) accurately track navigation history
- navstack_depth from Summary matches Naventry PoolCensus n_record exactly
- GoBack correctly pops navstack (n_record decrements)
- Overlay stacking works: detail then help produces 2 OverlayEntry records with LIFO semantics
- FPanel sel_row matches Summary PanelState sel_row exactly after navigation
- LeftItem count changes on namespace expand (83→213 when dmmeta expands)

### Filter Behavior (6 tests)
- SQL `%` wildcard matches all ctypes (7805 lines with records)
- Exact match (`acr_nav.FPanel`) returns correct records
- Partial match (`%Panel%`) equivalent to exact match
- SQL `_` single-char wildcard works (`acr_nav.FN_` matches FNs)
- `".*"` correctly treated as literal (not regex), matches nothing
- Empty filter matches nothing (census-only)

### Cross-Interface Consistency (4 tests)
- Batch vs headless census identical except: OverlayEntry (0 vs 1, expected), headless_lineno (0 vs 1)
- IPC single request returns correct records with `\n\n` sentinel
- IPC multi-request on same connection works (both FPanel and FViewmode)
- IPC census differs by 1 active connection (cd_ipcconn_read: 0→1, expected)

### Cross-Validation with acr (4 tests)
- FCtype: dump=1425, acr=1480 (acr_nav loads subset — reasonable)
- FField: dump=5738, acr=5959 (reasonable subset)
- FNs: dump=85, acr=89 (reasonable subset)
- FViewmode: dump=10, acr=10 (exact match)
- FKeybind: dump=42, acr=42 (exact match)

### Edge Cases (6 tests)
- Large dumps: FField 5738 records matches census count exactly
- Large dumps: FCtype 1425 records matches census count exactly
- Multiple dumps in one session: state correctly reflects navigation between dumps
- Filter matching nothing: census-only (22 lines), no record lines
- Malformed input: missing filter → treated as empty, extra fields → InputError, invalid command → InputError, empty line → silently ignored
- samp_meng cross-namespace: dump works, FDb singleton present

## Key Observations

1. **Filters are SQL wildcards, not regex.** The existing test `acr_nav.DumpAll` uses `".*"` which matches nothing — it only tests census lines, never record-level dumps. `"%"` is the correct match-all wildcard.

2. **navstack_depth = Naventry pool count.** Current location is NOT on the stack — only previous locations are pushed. After 2 Navigate calls, depth is 1.

3. **Overlay stacking is order-sensitive.** Help overlay blocks other viewmode keys. To stack, activate detail first, then `?` on top.

4. **Full dump is ~1 MB / 7800 lines.** Performance is good — completes in well under 1 second.

5. **IPC requires `-headless` when no terminal is available.** In containerized/CI environments, use `acr_nav -headless -ipc`.

6. **Headless auto-appends Screen output.** After RequestStateDump, headless also emits Screen/PanelState/VisibleLeftItem records (65 extra lines). This is the Screen refresh protocol, distinct from statedump.

## Regression Tests

```
atf_comp acr_nav.Dump%  →  2/2 pass (DumpAll, DumpHeadless)
```
