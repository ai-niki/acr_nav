# Plantable Bugs for Demo

**Orchestrator-only.** Subagents must never see this file.

Each bug was originally found by the agent-test skill during exploratory testing, then fixed. Re-introducing it creates a known defect the subagent must discover blind.

---

## Bug A: Filter Cancel Destroys State

**Category label:** State management
**Category description:** A bug in how acr_nav saves and restores internal state across mode transitions.
**Original severity:** High (data loss)
**Git fix:** `8f96c8e`

### What to change

**File:** `cpp/acr_nav/nav.cpp`

**In `navaction_filter_start()`** (grep: `void acr_nav::navaction_filter_start`):
Remove the 4 lines that save pre-filter state. The current code saves `pre_filter_text`, `pre_filter_target`, `pre_filter_sel_row`, `pre_filter_scroll_offset` before clearing the filter. Delete these 4 save lines (the ones assigning to `pre_filter_*`).

**In `navaction_filter_cancel()`** (grep: `void acr_nav::navaction_filter_cancel`):
Replace the body after `SwitchToBrowse()` with:
```cpp
    ch_RemoveAll(acr_nav::_db.filter);
    BuildLeftItemsReset();
```
This removes the restore logic that puts back the saved filter text, target, selection, and scroll offset.

### Expected symptom

1. Apply a filter (e.g., type "dmmeta", press Enter to accept)
2. Start a new filter (press `/`)
3. Cancel the new filter (press Escape)
4. **Expected:** filter restores to "dmmeta", selection returns to previous position
5. **Buggy:** filter is empty, selection resets to top. `Screen.filter` shows `""` instead of `"dmmeta"`.

### Subagent exploration hint

> Explore filter lifecycle: starting a filter, typing characters, accepting with Enter, canceling with Escape. Test what happens when you cancel a filter that was started while a previous filter was active. Verify that filter state, selection position, and filtertarget are preserved correctly across mode transitions.

---

## Bug B: Help Overlay Blocked on Namespace Rows

**Category label:** Overlay interaction
**Category description:** A bug in how overlay viewmodes (help, detail) interact with context-sensitive views on different row types.
**Original severity:** Medium
**Git fix:** `bb61b5a`

### What to change

**File:** `cpp/acr_nav/nav.cpp`

**In `PopOverlayOnCtypeChange()`** (grep: `!IsNsDepMode() && !sel_ct && !acr_nav::_db.p_cur_viewmode->is_overlay`):
Remove `&& !acr_nav::_db.p_cur_viewmode->is_overlay` from the condition on the line that starts nsdep auto-activation. The line currently reads:
```cpp
    if (!IsNsDepMode() && !sel_ct && !acr_nav::_db.p_cur_viewmode->is_overlay) {
```
Change to:
```cpp
    if (!IsNsDepMode() && !sel_ct) {
```

### Expected symptom

1. Navigate to a namespace header row (e.g., select "dmmeta" in the left panel)
2. Press `?` to invoke help overlay
3. **Expected:** `Screen.viewmode` shows `"help"`
4. **Buggy:** nsdep auto-activation overrides the help overlay. `Screen.viewmode` shows `"nsdep"` instead of `"help"`. Repeated `?` presses accumulate orphaned viewmode_stack entries.

### Subagent exploration hint

> Test overlay viewmodes (help via `?`, detail via `d`) across different selection contexts. Verify overlays work on both ctype rows and namespace header rows. Check that viewmode correctly reflects the overlay state after each keypress. Look for inconsistencies in how overlays behave depending on what's selected in the left panel.

---

## Bug C: Scroll Offset Not Capped on Terminal Grow

**Category label:** Viewport handling
**Category description:** A bug in how the viewport adjusts when terminal dimensions change.
**Original severity:** Medium
**Git fix:** `bb61b5a`

### What to change

**File:** `cpp/acr_nav/util.cpp`

**In `AdjustScroll()`** (grep: `void acr_nav::AdjustScroll`):
Remove the last two lines of the function body:
```cpp
    int max_offset = i32_Max(0, n_items - visible);
    panel.scroll_offset = i32_Min(panel.scroll_offset, max_offset);
```

### Expected symptom

1. Set a small terminal height (e.g., `SetTermSize term_hei:10 term_wid:80`)
2. Scroll to the bottom of a long list (press `G` or `End`)
3. Grow the terminal (e.g., `SetTermSize term_hei:50 term_wid:80`)
4. Take a screenshot
5. **Expected:** `PanelState.scroll_offset` decreases so items fill the larger viewport
6. **Buggy:** `scroll_offset` stays high, leaving blank space at the bottom. The offset will exceed `n_items - visible_rows`, which is invalid.

### Subagent exploration hint

> Test viewport behavior with SetTermSize. Start with a small terminal, scroll to various positions, then grow the terminal. Verify that scroll_offset and visible items adjust correctly after resize. Check that PanelState.scroll_offset never exceeds what's valid for the current viewport size and item count. Try both growing and shrinking the terminal at different scroll positions.
