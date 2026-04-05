# Runtime State Inspection (generalized headless)

This is an amc-level capability, not specific to any one tool.

## The idea

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. amc now generates a generic state-dump function (`dmmeta.nsdump`), so any OpenACR program can expose its full in-memory state on demand -- headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**End goal framing:** "make any amc program a glass box." Not SQL at runtime -- that would require a runtime query interpreter (an anti-pattern: interpreter adds complexity, not factorization). Instead: the program emits structured text; the consumer (agent or unix tools) filters and queries client-side. The query engine is the consumer.

**Value:** High for programs under active development. Mature tools (amc, acr) are already debugged -- the payoff is for new servers and services being built, where Claude Code needs to inspect evolving runtime state daily.
**Status:** Phases 1-3.4.3 done. Two generators (`ns_state_dump`, `ns_ipc`), one namespace (acr_nav), live inspect viewmode with columnar formatting, reference following, FDb detail card, and finput pool filtering. Remaining: input interface (Phase 4), meaningful second app (Phase 5). samp_meng integration removed — it was a smoke test, not a diagnostic showcase.
**Primary consumer:** Claude Code as agent -- inspecting programs at runtime during development, and auto-testing them similar to acr_nav headless. Any new amc program built with Claude Code benefits automatically -- no adoption curve.

## What generalizes cleanly (output/dump side)

amc knows every pool in FDb via the `zd_inst` linked list (discovery in `cpp/amc/gen.cpp:460`, `gen_detectinst()` checks `reftype.inst` flag). ~120 pool-typed fields (inst:Y reftypes) exist across all FDb structs — Lary accounts for ~116 of these. `amcdb.gen gen:ns_state_dump` generates a StateDump function for any namespace with `dmmeta.nsdump` — one record in `gen.ssim`, one generator. Passes the factorization test.

**Print gap:** Only 830/1468 ctypes have `cfmt print:Y`. Solved in Phase 2: cfmt for pools that have it, field-by-field serialization for the rest.

## Consumer-side filtering

The dump output is ssim: labeled key:value pairs, one record per line, type-prefixed:

```
report.PoolCensus   ctype:acr_nav.FCtype    n_record:42
report.PoolCensus   ctype:acr_nav.FPanel    n_record:2
report.IndexCensus  field:acr_nav.FDb.cd_ipcconn_read  ctype:acr_nav.FIpcconn  n_record:0
acr_nav.FCtype      ctype:dmmeta.Field      comment:""  ...
acr_nav.FPanel      side:left  sel:7  ...
```

Filterable at the consumer with no server-side query language:

```bash
acr_nav -dump:".*" | grep "^acr_nav.FCtype"       # filter by ctype
acr_nav -dump:".*" | grep "reftype:Lary"           # filter by field value
acr_nav -dump:".*" | grep "n_record:0"             # find empty pools/indexes
acr_nav -dump:".*" | grep "IndexCensus"            # all index queue depths
```

For complex predicates, the agent processes the dump programmatically. Same model as acr: load everything, filter client-side.

## What doesn't generalize (input/step side)

`SendKey` is an acr_nav verb -- meaningful because it's a TUI with keybinds. A batch tool has args + stdin. A server has protocol messages. The "dump" verb is universal; the "step" verb is program-specific.

**Potential generalization via `dmmeta.rtquery`:** Declare the program's input interface (what commands it accepts) in ssimfiles, symmetric with `nsdump` for output:

```
dmmeta.nsdump   ns:acr_nav                    → generates StateDump()    — output side
dmmeta.rtquery  rtquery:acr_nav.Navigate      → generates dispatch case  — input side
```

amc would generate the dispatch scaffolding (try-deserialize-call-handler loop); the handler bodies remain hand-written. Value: the interface becomes machine-readable -- `acr dmmeta.rtquery ns:acr_nav` tells an agent what commands a program accepts without reading source code.

**Check first:** `dmmeta.dispatch` + `dmmeta.dispatch_case` may already cover this. Verified: `atf_amc.Ssimfiles` with `read:Y` generates exactly the "try-deserialize-call-handler" loop. `rtquery` might be just a semantic marker on existing dispatch.

## Where the value is highest

**Long-running servers under active development.** This is where inspectability pays off most:

| Property | Why census helps |
|---|---|
| **Long-running** | State accumulates -- leaks, stuck messages, orphaned connections are invisible without inspection |
| **Multi-protocol** | Need to verify state at each protocol boundary (ingress vs egress) |
| **Concurrent** | Multiple producers/consumers -- counts reveal imbalances that logs bury |
| **Tiered storage** | Pool counts show where data lives right now (memory vs disk vs object store) |
| **Multi-tenant** | Per-tenant pool counts reveal isolation violations or resource hogging |

The diagnostic scenario: a consumer receives 97 of 100 messages. Where are the other 3? Pool census answers in seconds -- `FMessage n:100, FDelivery n:97, FPendingAck n:3` -- problem is in the ack path, not ingestion. No rebuild, no reproduce, no log archaeology.

For mature batch tools (amc, acr, abt), the value is lower -- they already work. The investment pays off for programs that are *being built*, not programs that are *already built*.

## Curated vs raw

Generate the raw dump (free from schema); programs can also define curated views as additional ctypes. Both, not either/or. Curated views are just more records — they pass the factorization test.

## Tpool traversal (verified)

Full pool scan has a practical obstacle: Tpool generates zero cursor functions (confirmed: not in `amcdb/tcurs.ssim`, no iteration in `cpp/amc/tpool.cpp`). Free-list allocator -- can alloc/free but not scan. Lary, Tary, Ptrary, Inlary all have cursors; Tpool and Sbrk do not.

The right approach: **dump everything reachable**, not everything allocated. Follow access paths from `_db`, not scan pools. For pools with cursors (Lary, Tary, etc.) -- direct iteration. For Tpool records -- reach them via xref indexes (Thash, Llist) hanging off `_db`. Output as ssim tuples with regex filtering.

## Trigger mechanism

Depends on program type and phase:

| Program type | Trigger | Output channel |
|---|---|---|
| Batch tool | `-dump` flag, emits state before exit | file or stderr |
| TUI (headless) | stdin command (proven with acr_nav headless) | stdout interleaved |
| TUI (live) | IPC socket -- B connects to running A, polls StateDump | socket response |
| Long-running server | IPC socket or SIGUSR1 | socket response or file |

**IPC transport (`dmmeta.nsipc`):** For live inspection of a running program, a Unix domain socket is the right mechanism. A listens on a well-known socket path. B connects, sends `acr_nav.RequestStateDump` commands, reads responses. Multiple clients can connect. A's event loop polls the socket fd alongside the keyboard fd -- no new thread, no blocking.

Two records, two concerns:

```
dmmeta.nsdump  ns:acr_nav                     # generates StateDump()        — what to dump
dmmeta.nsipc   ns:acr_nav                     # generates socket listener    — how to trigger
```

amc generates the socket setup and poll integration. The developer wires it into their event loop -- one call in the poll branch, same pattern as the existing keyboard dispatch.

## The live debugger demo

Two terminal panes. Left: acr_nav A, user navigating schema in TUI. Right: acr_nav B, connected to A via socket, polling StateDump every ~100ms.

As the user navigates in A:
- B shows `FNavstack n_record` increment as they drill into references
- B shows `FPanel.sel` updating as they scroll
- B shows `FFilter` changing as they type

A is not frozen. A is running, responding to input. B watches A's pools change in real time -- no breakpoints, no logging code, no instrumentation added to A. The schema is the instrumentation.

This is the "freeze a running program and look at its tables" mental model made literal -- except without the freeze.

## acr_nav as runtime explorer

acr_nav already consumes ssim as its data model. State dump output is also ssim. acr_nav can load a state dump as a live data layer alongside the schema: same navigator, two views — schema structure on the left, live instances on the right. One tool, complete picture.

**Proven in Phase 3.4:** acr_nav loads all of dmmeta at startup, so B already knows A's ctypes. The `inspect` viewmode displays live pool data. The recursive case works: one acr_nav inspecting another shares the same type system.

**Remaining work:** columnar formatting, reference-following, static pool filtering (see Phase 3.4.2).

## Phased implementation

### Phase 1 -- Pool census (done)

Generate a function that emits ctype name + record count for every pool in FDb. One line per pool. Zero serialization, zero Print dependency. Lary covers ~116 of ~120 pools so `_n` handles almost everything. Output: `report.PoolCensus ctype:ns.FCtype n_record:423`. Smallest useful increment. Single `amcdb.gen` record + one generator function.

**Implemented:** `amcdb.gen gen:ns_state_dump perns:Y`, generator in `cpp/amc/state_dump.cpp`. Opt-in via `dmmeta.nsdump ns:<ns>`. Enabled for acr_nav.

### Phase 2 -- Filtered record dump (done)

Add regex filtering on ctype name. For matching pools, iterate records and emit as ssim tuples. Pools with `cfmt print:Y` use the cfmt printer. Pools without cfmt get field-by-field serialization generated directly from field definitions (Val, Smallstr, Bitfld, Regx fields). `StateDumpFieldQ()` mirrors GenPrintField's structural checks.

**Implemented:** Same generator (`cpp/amc/state_dump.cpp`). CLI trigger: `acr_nav -dump:"<regex>"`. Headless trigger: `acr_nav.RequestStateDump filter:"<regex>"`. 830/1468 ctypes have cfmt; remaining get field-by-field serialization where fields have print support.

### Phase 3 -- IPC transport (done, MVP)

Unix domain socket so an external process connects to a running program and requests StateDump without restarting it.

**What was built:**

Schema: `dmmeta.nsipc ns:<ns>` opt-in record. Per-namespace infrastructure in ssimfiles: `FIpcconn` ctype (Linebuf fbuf + outfd), `FDb` fields (Tpool pool, Llist ready/eof, FIohook listen, cstring socket_path), fstep Inline records, `-ipc` bool command flag, dispatch record (`read:Y` for queryability).

Generator: `amcdb.gen gen:ns_ipc perns:Y` in `cpp/amc/ipc.cpp`. Generates 4 function bodies: `cd_ipcconn_read_Step` (RotateFirst + dispatch), `IpcProcessLine` (parse RequestStateDump, call StateDump, write response), `cd_ipcconn_eof_Step` (close fd, delete connection), `IpcCleanup` (unlink socket). Two functions are extern (user-written): `IpcInit` (create/bind/listen socket, register with epoll) and `IpcAccept` (accept connection, allocate FIpcconn, start fbuf read). Extern because they call `lib_netio` which lives behind the gen/hand-written header boundary.

Socket path: `/tmp/<ns>.<pid>.sock`. PID-based for multi-instance safety. Discovery via `ls /tmp/<ns>.*.sock`.

Protocol: ssim over Linebuf. Client sends `<ns>.RequestStateDump filter:"<regex>"\n`, server responds with StateDump output.

Activation: `-ipc` flag enters `MainLoop()` as a pure IPC server. Headless and TUI integration added in Phases 3.1 and 3.3.

**Verified:** Single client, multi-client (2 simultaneous), 66 component tests pass.

**Key files:** `cpp/amc/ipc.cpp` (generator), `cpp/acr_nav/ipc.cpp` (hand-written IpcInit/IpcAccept), `cpp/lib_netio/socket.cpp` (CreateUnixSocket, BindUnix, AcceptUnix).

**Lessons learned during implementation:**

1. *Generator ordering matters.* A `perns:Y` generator runs after field processing (`gen:prep_field`, `gen:ns_tclass_field`). It cannot create synthetic ctypes/fields that need fbuf, Llist, fstep, or Tpool code generation. All structural records must be explicit in ssimfiles. The generator only writes function bodies.

2. *Gen/hand-written header boundary.* Generated code in `cpp/gen/` only includes `include/gen/*.h`. Hand-written headers like `include/lib_netio.h` are unreachable. Functions that call across this boundary must be extern (prototype generated, body hand-written).

3. *Tpool, not Lary* for connection pools. Clients connect and disconnect. Lary is append-only (no Delete). Tpool provides Alloc/Delete. Connections are accessed via Llist, not pool scan.

4. *Custom event loops bypass IPC.* HeadlessMain uses blocking `read()` in a while loop. TUI mode uses its own rendering loop. Neither calls `algo_lib::MainLoop()` / `giveup_time_Step()`. IPC step functions only fire from MainLoop. Integration requires reworking those loops.

### Phase 3.1 -- IPC + headless integration (done)

When `-headless -ipc` are both set, stdin is registered as a non-blocking FIohook with epoll alongside the IPC listen socket. Both polled by `algo_lib::MainLoop()`. Stdin dispatches through `DispatchHeadlessCommand()`; IPC through generated `IpcProcessLine()`. Standalone headless (without `-ipc`) preserves the simpler blocking `read()` loop.

**Key files:** `cpp/acr_nav/main.cpp` (StdinReadCallback, HeadlessIpcInit).

**Lessons learned:**

5. *Edge-triggered epoll requires complete drain.* Read until EAGAIN or EOF. If a quit command sets `_db.running = false` mid-drain, must still call `ReqExitMainLoop()` or MainLoop hangs.

### Phase 3.2 -- Hardening (done)

Signal handler (generated `IpcSignalHandler`, async-signal-safe cleanup), BindUnix error checking, Zeroterm fix for socket path. Previously validated on `samp_meng` as second namespace (zero generator changes required); samp_meng integration since removed as it was a smoke test, not a diagnostic showcase.

**Stale socket detection deferred:** PID-based naming makes collisions nearly impossible; `unlink`-before-`bind` handles stale sockets.

**Key files:** `cpp/amc/ipc.cpp`, `cpp/acr_nav/ipc.cpp`.

**Lessons learned:**

7. *cstring is NOT null-terminated.* Use `Zeroterm()` before passing to C functions expecting `const char*`.

8. *dispatch `textcall:Y` vs `read:Y`.* `textcall` generates `DispatchText(ctx, line)` with context parameter — needed for IPC where the handler needs the connection. `read:Y` generates `ReadStrptrMaybe` without context.

9. *`dispctx` record required* for `textcall` dispatches. Without it, `DispatchText` is not generated.

### Phase 3.3 -- TUI + IPC integration (done)

A's side of the "live debugger demo." When `-ipc` is set, replaces blocking `ReadKeyName()` with epoll-driven `MainLoop()`. `DecodeKeyByte()` is a non-blocking VT100 state machine; `TuiStdinReadCallback()` drains bytes and batches repaints. 50ms timehooks handle bare-ESC detection and SIGWINCH. Blocking TUI loop preserved when `-ipc` is not set.

**Also added:** FDb singleton dump in `gen_ns_state_dump()` — scalar Val/Ptr fields from `_db` itself, before the pool loop.

**Key files:** `cpp/acr_nav/main.cpp` (TuiIpcInit, DecodeKeyByte, TuiStdinReadCallback), `cpp/amc/state_dump.cpp`.

**Lessons learned:**

10. *Rebuild amc before running amc.* When modifying a generator, `abt -build -install amc` first. `ai` bootstraps but may use the stale binary for the first pass.

11. *Raw byte drain.* Same edge-triggered pattern as Phase 3.1, but with raw bytes. Partial ESC sequences persist across reads via `acr_nav_keybuf`.

### Phase 3.4 -- Live data viewmode (B's side) (done, MVP)

B's side of the "live debugger demo." `inspect` viewmode connects to A's IPC socket, polls `RequestStateDump` every 100ms with backpressure, displays pool state live.

**Usage:**
```bash
# Terminal A
acr_nav -ipc

# Terminal B
acr_nav -connect /tmp/acr_nav.<pid>.sock
```

B shows A's pool state updating live. Select `acr_nav.FPanel` for panel state, `acr_nav.FDb` for global state.

Client: `LiveConnect()` + `LivePollCallback()` (100ms with backpressure) + `LiveReadCallback()` (drains to staging buffer, scans for `\n\n` sentinel). Server: write loop for non-blocking fd + `\n\n` response framing.

**Key files:** `cpp/acr_nav/main.cpp` (LiveConnect, LivePollCallback, LiveReadCallback), `cpp/acr_nav/content.cpp` (viewmode_inspect_ensure_content), `cpp/acr_nav/ipc.cpp` (response framing), `cpp/lib_netio/socket.cpp` (ConnectUnix).

**Lessons learned:**

12. *Non-blocking write truncates.* Single `write()` exceeds ~212KB socket buffer. Fix: write loop retrying on EAGAIN.

13. *LineBuf is per-buffer, not per-stream.* Fix: accumulate into persistent staging cstring, scan for sentinel after drain.

14. *Response framing is mandatory for SOCK_STREAM.* Fix: `\n\n` sentinel appended by server, detected by client.

15. *Report types have no pkey field.* `report.PoolCensus` and `report.IndexCensus` are protocol messages, not database records.

16. *Guard generated variable declarations.* Gate behind count check to avoid `-Wunused-variable` for namespaces with zero matching fields.

### Phase 3.4.1 -- Inspect viewmode redesign (done)

Replaced the hacky MVP with a proper pool-driven inspect mode:

- **Universal connectivity:** Parse namespace from socket path `/tmp/<ns>.<pid>.sock`, construct `<ns>.RequestStateDump` dynamically. Connect to any amc app, not just acr_nav.
- **Pool-driven left panel:** Built entirely from PoolCensus data. Shows only live pools with record counts: `FPanel (2)`, `FViewmode (10)`. FDb singleton detected and included as synthetic entry.
- **Tab locked to inspect:** Other viewmodes (fields, xref, codegen) show static schema irrelevant in connect mode.
- **Proper schema:** `PoolEntry` ctype with Tary on FDb (dynamic, no fixed ceiling). `bool live_mode` on FDb replaces scattered string-length checks. Parse validation with diagnostic on malformed socket path.

**Key files:** `cpp/acr_nav/nav.cpp` (BuildLiveLeftItems), `cpp/acr_nav/main.cpp` (TuiLiveInit, LivePollCallback), `cpp/acr_nav/render.cpp` (live mode rendering + width).

### Phase 3.4.2 -- Inspect viewmode UX polish (done)

**Gap A: Columnar formatting.** Inspect view now parses live data tuples and formats aligned columns, reusing `FormatPreviewRow`, `BuildPreviewHeader`, and a shared `MeasureTupleColumns` helper extracted from preview mode. Two-pass pipeline: measure column widths across matching records, then format. Census/IndexCensus lines appended as unformatted comments below records.

**Gap B: Follow references from inspect view.** `DetectNavColumns` parameterized with `bool live_mode` — in live mode, FK detection checks pool_entry membership instead of ssimfile existence, making Ptr fields navigable when their target pool exists. `BuildLeftItems` guarded for live mode (SESE wrap), so `RevealCtype` → `NavigateToTarget` works for both modes with zero duplication. Enter-follow jumps between live pools; Backspace returns via navstack.

**Rendering generalization:** Four hardcoded preview viewmode identity checks in render.cpp replaced with `nav_col_N(*p_cur_viewmode) > 0` capability checks: h-scroll activation, h-scroll adjustment, nav-cell overlay, and styled column header. Any viewmode with nav_col entries now gets columnar rendering.

**Key files:** `cpp/acr_nav/content.cpp` (MeasureTupleColumns, FindPoolEntry, DetectNavColumns live_mode, viewmode_inspect_ensure_content), `cpp/acr_nav/render.cpp` (AdjustHScroll, DetectNavOverlay, RenderColumnHeader, Render), `cpp/acr_nav/nav.cpp` (BuildLeftItems live guard).

**Gap C: Static schema data dominates dump.** `acr_nav.FCtype` (1423 records) and `acr_nav.FField` (5729 records) are loaded from disk at startup and never change. They dominate the dump output. Solved in Phase 3.4.3 via finput pool filtering.

### Phase 3.4.3 -- FDb detail card and finput filtering (done)

Three UX improvements to the inspect viewmode:

**FDb detail card format.** When FDb is selected in the left panel, the right panel renders a vertical detail card (one field per line, key-aligned) instead of columnar format. FDb is a singleton — columnar layout wastes space. `FormatDetailCard()` handles the vertical layout, skipping the primary key attr when redundant with the section header.

**Finput pool filtering.** Pools backed by finput (loaded from disk at startup: FCtype, FField, FSsimfile, etc.) are static schema data, not interesting runtime state. The state dump generator tags census lines with `is_finput:Y`. On wildcard filter, the dump skips record-level output for finput pools (census line still emitted). In the inspect viewmode, `BuildLiveLeftItems` hides finput pools by default.

**TUI static toggle.** `s` key in browse mode toggles `live_show_static`, making finput pools visible/hidden in the left panel. Bound via `acr_navdb.keybind browse.s → filter_static`. The toggle also switches the RequestStateDump filter between `{ns}.%` (static included) and `%` (default, finput skipped on wildcard).

**Inspect removed from Tab cycle.** Inspect viewmode is only meaningful in live connect mode. Removed from the Tab rotation so it doesn't appear when browsing schema normally.

**Key files:** `cpp/acr_nav/content.cpp` (FormatDetailCard, viewmode_inspect_ensure_content FDb branch), `cpp/acr_nav/nav.cpp` (BuildLiveLeftItems finput guard, navaction_filter_static), `cpp/acr_nav/main.cpp` (LivePollCallback filter logic), `cpp/amc/state_dump.cpp` (finput skip on wildcard, is_finput tag).

### Phase 4 -- Input interface (`dmmeta.rtquery`)

Declare program commands in ssimfiles, generate dispatch scaffolding. Currently the IPC dispatch uses direct `RequestStateDump_ReadStrptrMaybe()` -- adding a `dispatch_msg` record is a silent no-op. Refactor to dispatch `call:Y` so dispatch_msg records drive code generation. Investigate whether `dmmeta.dispatch` already covers this (10 dispatch records exist; `atf_amc.Ssimfiles` with `read:Y` generates exactly the "try-deserialize-call-handler" loop).

### Phase 5 -- Second namespace: prove it generalizes on a meaningful app

Phase 3.2 validated that the generators work on a second namespace (samp_meng) — zero generator changes required, only ssim records + two extern functions. That integration was removed since samp_meng was a smoke test, not a program where pool inspection has real diagnostic value.

Phase 5 requires enabling `nsdump` + `nsipc` on a program with meaningful runtime state — a long-running server or stateful tool where inspectability pays off (see "Where the value is highest" section). The generators are proven generic; what remains is demonstrating diagnostic value on a real workload.

**Candidate criteria:** multiple pools with dynamic record counts, state that accumulates over time, diagnostic scenarios where census answers questions that logs cannot.

### Beyond Phase 5 -- Possible extensions

Ideas that don't have phases yet. Each would need a use case before committing:

- **Richer query filtering.** Current filter is a ctype-name regex. Field-level predicates (e.g., "show FConnection where state:idle") would let agents ask sharper questions. But this edges toward a runtime query interpreter — an anti-pattern per the vision. The consumer-side filtering via grep/agent is the intended model. Only revisit if consumer-side proves insufficient in practice.
- **`dmmeta.rtquery` as semantic marker.** Even without Phase 4's full dispatch generation, a simple marker record saying "this program accepts RequestStateDump over IPC" would let agents discover capabilities via `acr dmmeta.rtquery ns:acr_nav` without reading source code.
- **Differential dumps.** Send only what changed since the last poll, not the full state. Useful when B polls A at high frequency and the state is large. Would require sequence numbers or checksums per pool. Premature until Phase 3.4 reveals whether full dumps are actually a bottleneck.
- **IpcInit/IpcAccept generation.** Currently these are extern (hand-written) because they call `lib_netio` which lives behind the gen/hand-written header boundary. If the generator could emit them directly (e.g., by adding `lib_netio` includes to the generated header), the two extern functions disappear and onboarding a new namespace becomes pure ssim records — zero hand-written code.

## StateDump coverage gaps (identified 2026-04-04)

Discovered during Phase 3.3 live testing. Systematic audit of all FDb field reftypes to identify what StateDump misses.

### Gap 1: Ptr fields on FDb — "what am I looking at" (DONE)

Generator in `cpp/amc/state_dump.cpp` now has a `ResolvePtrPkey` helper that emits Ptr field pkeys (NULL-guarded) in the FDb singleton dump section. All 12 Ptr fields on acr_nav.FDb are now visible in the state dump:

| Field | What it tells you |
|---|---|
| `p_cur_panel` | Which panel is focused |
| `p_left_panel` | Left panel (ctype list) |
| `p_right_panel` | Right panel (content) |
| `p_cur_mode` | Browse or filter mode |
| `p_filter_mode` | Cached pointer to filter navmode |
| `p_cur_viewmode` | Current view (fields, summary, detail, xref, etc.) |
| `p_default_viewmode` | Default viewmode (fields) |
| `p_detail_field` | Which field is being detailed (null outside detail view) |
| `p_cur_filtertarget` | What the filter targets (ctype, field, ns, etc.) |
| `p_default_filtertarget` | Cached pointer to ctype filtertarget |
| `p_nsdep_ns` | Namespace in nsdep dependency view |
| `p_pre_nsdep_viewmode` | Viewmode saved before nsdep context switch |

### Gap 2: Tary pools (ALREADY DONE)

Generator handles Tary alongside Lary and Inlary. Census and dump both work for navstack, left_item, overlay_stack.

### Gaps 3, 7: Upptr/Delptr on FDb — N/A

Zero Upptr and zero Delptr fields exist on any FDb. Covered when owning pool's records are dumped.

### Gaps 4, 5: Ptrary + Bheap index census (DONE)

Secondary indexes (not pools — records owned by Lary pools). Census via `report.IndexCensus` emits field + ctype + count. 15 Ptrary + 16 Bheap FDb fields across all programs; zero on current nsdump namespaces. Code ready for future programs.

### Gap 6: Tpool pools — census via Llist counts

Tpool has no cursor (free-list allocator). Records reachable via Llist. Census addressed by Gap 8: Llist `havecount:Y` fields emit IndexCensus lines. Record dump deferred — Llist iteration is partial view (misses records between alloc and list insert).

### Gap 8: Llist index census (DONE)

Llist fields with `havecount:Y` emit `report.IndexCensus` lines. 78 Llist FDb fields total, 64 with `havecount:Y`. New `report.IndexCensus` ctype (fields: `field`, `ctype`, `n_record`). Consumer in `cpp/acr_nav/content.cpp` displays in inspect viewmode.

### Coverage summary (updated 2026-04-05)

Pool-type coverage:

| Pool reftype | Has cursor? | Has `_N()`? | Covered? | Gap |
|---|---|---|---|---|
| Lary | Yes | Yes | **Yes** | — |
| Inlary | Yes | Yes | **Yes** | — |
| Tary | Yes | Yes | **Yes** | — |
| Tpool | No | No | Partial | Gap 6: census via Llist; record dump deferred |
| Lpool | No | No | No | Infrastructure allocator, skip |
| Sbrk | No | No | No | Infrastructure allocator, skip |
| Malloc | No | No | No | Infrastructure allocator, skip |
| Blkpool | No | No | No | Infrastructure allocator, skip |
| Global | N/A | N/A | **Yes** | FDb singleton section |
| Cppstack | N/A | N/A | N/A | Stack-scoped, no runtime records |

Index-type coverage (via `report.IndexCensus`):

| Index reftype | Has `_N()`? | Covered? | Notes |
|---|---|---|---|
| Ptrary | Yes (unconditional) | **Yes** | Census only; records in owning pool |
| Bheap | Yes (unconditional) | **Yes** | Census only; records in owning pool |
| Llist | Conditional (`havecount:Y`) | **Yes** | Census for 64/78 FDb fields |
| Thash | Yes (unconditional) | No | Redundant with PoolCensus (count == pool count) |
| Atree | No | No | Tree index internals, low priority |

FDb scalar/pointer field coverage:

| Field reftype | Covered? | Notes |
|---|---|---|
| Val, Smallstr, Bitfld, Regx | **Yes** | — |
| Ptr | **Yes** | — |
| Upptr | N/A | Zero FDb fields exist |
| Delptr | N/A | Zero FDb fields exist |
| Count | No | Emittable as i32 if added |

### Remaining gaps

**Tpool record dump (Gap 6 residual):** Census tells you queue depths but not record contents. Options: (a) iterate Llist cursors for Tpool-backed ctypes, accepting partial view; (b) add Tpool cursor to amc (invasive); (c) defer until a use case demands it.
**Count fields on FDb:** Low priority. Emittable as i32 via StateDumpFieldQ extension.
**Thash census:** Has `_N()` but count is redundant with PoolCensus (every indexed record is also in a pool). Only useful for detecting indexing inconsistencies (count mismatch between pool and hash). Not worth the noise for diagnostics.
**Atree census:** No `_N()` accessor. Would require tree traversal. Low priority.

## Generator mechanics (verified, updated 2026-04-04)

95 generators exist in `data/amcdb/gen.ssim`. Two added for state dump: `gen:ns_state_dump` (Phase 1+2) and `gen:ns_ipc` (Phase 3), both `perns:Y`. Adding a new generator requires: 1 ssimfile record, 1 C++ function, 1 targsrc record. Uniform `void()` contract. Generator ordering matters: `ns_ipc` must come after `ns_state_dump` (depends on StateDump existing) and before `ns_funcindex` (which prints function bodies).

Key constraint: `perns:Y` generators run after all field/ctype processing. They cannot create synthetic structural records (ctypes, fields, fbuf, fstep). All structure must be declared in ssimfiles. Generators only fill in function bodies. Only `perns:N` generators like `gen:dispenum` run early enough to create synthetic ctypes.
