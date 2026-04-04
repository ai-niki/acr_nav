# Runtime State Inspection (generalized headless)

This is an amc-level capability, not specific to any one tool.

## The idea

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. If amc could generate a generic state-dump function, any OpenACR program could expose its full in-memory state on demand -- headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**End goal framing:** "make any amc program a glass box." Not SQL at runtime -- that would require a runtime query interpreter (an anti-pattern: interpreter adds complexity, not factorization). Instead: the program emits structured text; the consumer (agent or unix tools) filters and queries client-side. The query engine is the consumer.

**Value:** High for programs under active development. Mature tools (amc, acr) are already debugged -- the payoff is for new servers and services being built, where Claude Code needs to inspect evolving runtime state daily.
**Size:** Medium. Phase 1 (pool census) is a single amc generator. Phase 2 (filtered dump) adds serialization. Phase 3 (IPC transport) enables live inspection.
**Primary consumer:** Claude Code as agent -- inspecting programs at runtime during development, and auto-testing them similar to acr_nav headless. Any new amc program built with Claude Code benefits automatically -- no adoption curve.

## What generalizes cleanly (output/dump side)

amc knows every pool in FDb via the `zd_inst` linked list (discovery in `cpp/amc/gen.cpp:460`, `gen_detectinst()` checks `reftype.inst` flag). ~120 pool-typed fields (inst:Y reftypes) exist across all FDb structs — Lary accounts for ~116 of these. A new `amcdb.gen` phase could generate a state-dump function -- one record in `gen.ssim`, one generator, every program gets it. Passes the factorization test.

**Print gap (verified 2026-04-02):** Print is NOT generated for every ctype. Only 830 of 1468 ctypes have `cfmt` entries with `print:Y`. 619 have no cfmt at all (mostly extern types like DIR, SSL_CTX). 19 explicitly opt out. MVP approach: census (counts) for all pools, record dump only for ctypes that have cfmt. Full dump would require field-by-field serialization independent of cfmt -- Phase 2 work.

## Consumer-side filtering

The dump output is ssim: labeled key:value pairs, one record per line, type-prefixed:

```
report.PoolCensus  ctype:acr_nav.FCtype    n_record:42
report.PoolCensus  ctype:acr_nav.FPanel    n_record:2
acr_nav.FCtype     ctype:dmmeta.Field      comment:""  ...
acr_nav.FPanel     side:left  sel:7  ...
```

Filterable at the consumer with no server-side query language:

```bash
acr_nav -dump:".*" | grep "^acr_nav.FCtype"       # filter by ctype
acr_nav -dump:".*" | grep "reftype:Lary"           # filter by field value
acr_nav -dump:".*" | grep "n_record:0"             # find empty pools
```

For complex predicates or cross-record correlation, the agent processes the dump programmatically -- reads ssim tuples, builds its own view, runs whatever analysis it needs. This is exactly how acr itself works: load everything, filter client-side. No runtime query engine needed; the query engine is the consumer.

The ssim format makes this natural: every field is labeled, every line is self-describing, type prefixes enable reliable filtering. No schema knowledge required at grep-time.

## What doesn't generalize (input/step side)

`SendKey` is an acr_nav verb -- meaningful because it's a TUI with keybinds. A batch tool has args + stdin. A server has protocol messages. The "dump" verb is universal; the "step" verb is program-specific.

**Potential generalization via `dmmeta.rtquery`:** Declare the program's input interface (what commands it accepts) in ssimfiles, symmetric with `nsdump` for output:

```
dmmeta.nsdump   ns:acr_nav                    → generates StateDump()    — output side
dmmeta.rtquery  rtquery:acr_nav.Navigate      → generates dispatch case  — input side
```

amc would generate the dispatch scaffolding (try-deserialize-call-handler loop); the handler bodies remain hand-written. Value: the interface becomes machine-readable -- `acr dmmeta.rtquery ns:acr_nav` tells an agent what commands a program accepts without reading source code.

**Check first, before creating any schema records:** `dmmeta.dispatch` + `dmmeta.dispatch_case` may already cover this. The dispatch system generates exactly this kind of "try each message type" loop — verified: `atf_amc.Ssimfiles` uses dispatch with `read:Y` and generates `_ReadStrptrMaybe` dispatch identical to what `rtquery` would produce. `rtquery` might be nothing more than a semantic marker (flagging dispatch cases as "agent interface") on an existing mechanism. Do not design a new table before exhausting `dmmeta.dispatch`.

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

Raw pool dumps are the truth -- curated views are opinions that drift. But raw dumps of a real server (thousands of records, runtime artifacts like file descriptors and computed caches) are a firehose. ACR answer: generate the raw dump (free from schema), let programs also define curated views as additional ctypes. Both, not either/or. The curated views are just more records -- they pass the factorization test.

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

**IPC transport (`dmmeta.nsipc`):** For live inspection of a running program, a Unix domain socket is the right mechanism. A listens on a well-known socket path. B connects, sends `acr_nav.StateDump` commands, reads responses. Multiple clients can connect. A's event loop polls the socket fd alongside the keyboard fd -- no new thread, no blocking.

Two records, two concerns:

```
dmmeta.nsdump  ns:acr_nav                     # generates StateDump()        — what to dump
dmmeta.nsipc   ns:acr_nav  transport:socket   # generates socket listener    — how to trigger
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

acr_nav already consumes ssim as its data model -- schema records ARE ssim (dmmeta.ctype, dmmeta.field). State dump output IS ssim (different ctypes, same format).

acr_nav could load a state dump as a live data layer alongside the schema: same navigator, two views -- schema structure on the left, live instances on the right. "FCtype has 42 records at runtime -- navigate into them." The tool already knows how to display ctypes and their fields; live data populates the counts with real numbers.

acr_nav for schema structure + acr_nav+statedump for runtime instances = one tool, complete picture.

**What's actually free:** acr_nav loads all of dmmeta at startup -- every program's ctypes, not just its own. So B already knows `acr_nav.FPanel`, `acr_nav.FNavstack`, `acr_nav.FCtype` etc. No dynamic schema loading needed. The format is the same ssim that acr_nav already parses.

**What's real work:** a new "live data" viewmode in acr_nav that displays live records alongside schema records. Currently acr_nav shows schema structure; it doesn't have a view for "here are the live instances of this type." That viewmode is the implementation cost -- not schema loading, not format conversion.

The recursive case: one acr_nav inspecting another running acr_nav. B displays A's FPanel, FNavstack, FFilter records live. B already knows these types -- they're its own schema. The inspector and the inspected share the same type system. This is the cheapest case -- same program, schema trivially shared.

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

Activation: `-ipc` flag enters `MainLoop()` as a pure IPC server. Headless/TUI modes do NOT integrate IPC yet (they have custom event loops that bypass algo_lib's epoll).

**Verified:** Single client, multi-client (2 simultaneous), 63 component tests pass.

**Key files:** `cpp/amc/ipc.cpp` (generator), `cpp/acr_nav/ipc.cpp` (hand-written IpcInit/IpcAccept), `cpp/lib_netio/socket.cpp` (CreateUnixSocket, BindUnix, AcceptUnix).

**Lessons learned during implementation:**

1. *Generator ordering matters.* A `perns:Y` generator runs after field processing (`gen:prep_field`, `gen:ns_tclass_field`). It cannot create synthetic ctypes/fields that need fbuf, Llist, fstep, or Tpool code generation. All structural records must be explicit in ssimfiles. The generator only writes function bodies.

2. *Gen/hand-written header boundary.* Generated code in `cpp/gen/` only includes `include/gen/*.h`. Hand-written headers like `include/lib_netio.h` are unreachable. Functions that call across this boundary must be extern (prototype generated, body hand-written).

3. *Tpool, not Lary* for connection pools. Clients connect and disconnect. Lary is append-only (no Delete). Tpool provides Alloc/Delete. Connections are accessed via Llist, not pool scan.

4. *Custom event loops bypass IPC.* HeadlessMain uses blocking `read()` in a while loop. TUI mode uses its own rendering loop. Neither calls `algo_lib::MainLoop()` / `giveup_time_Step()`. IPC step functions only fire from MainLoop. Integration requires reworking those loops.

### Phase 3.1 -- IPC + headless integration (done)

When `-headless -ipc` are both set, stdin is registered as a non-blocking FIohook with epoll alongside the IPC listen socket. Both are polled by `algo_lib::MainLoop()`. Stdin lines dispatch through `DispatchHeadlessCommand()` (all 9 command types). IPC connections dispatch through generated `IpcProcessLine()` (RequestStateDump).

**What was built:**

Schema: `acr_nav.FDb.stdin_iohook` (Val FIohook) for stdin epoll registration.

Code: `StdinReadCallback()` — edge-triggered epoll callback that drains stdin, feeds lines through `DispatchHeadlessCommand()`. `HeadlessIpcInit()` — sets stdin to non-blocking, registers iohook, initializes panels. `Main()` branching: new `ipc && headless` branch before standalone `ipc`, calls `HeadlessIpcInit()` then `MainLoop()`.

**Key files:** `cpp/acr_nav/main.cpp` (StdinReadCallback, HeadlessIpcInit, Main branching).

**Lessons learned:**

5. *Edge-triggered epoll requires complete drain.* The callback must read until EAGAIN or EOF. If a quit command sets `_db.running = false` mid-drain, the callback must still call `ReqExitMainLoop()` before returning, or MainLoop hangs waiting for an epoll event that will never fire.

6. *HeadlessMain preserved for standalone headless.* The blocking `read()` loop is simpler and correct when IPC isn't needed. No reason to force everything through epoll.

### Phase 3.2 -- Hardening (done)

**What was built:**

1. *SIGTERM/SIGINT signal handler.* Generated `IpcSignalHandler` in `cpp/amc/ipc.cpp` — calls `IpcCleanup()` then `_exit(1)`. Both async-signal-safe. Hand-written IpcInit installs via `sigaction`. Socket cleaned up on kill, not just normal exit.

2. *BindUnix error checking.* IpcInit now checks `BindUnix()` return value, calls `FatalErrorExit` with socket path on failure.

3. *Zeroterm fix.* IpcCleanup was calling `unlink(ch_elems)` on a non-null-terminated cstring. Fixed generator to use `Zeroterm()`. Hand-written IpcInit also calls `Zeroterm()` after assignment to ensure the string is pre-terminated for the signal handler.

4. *Second namespace: samp_meng.* Full IPC + state dump enabled for the sample matching engine. 29 schema records added (nsdump, nsipc, FIpcconn, Tpool, Llist, fbuf, dispctx, dispatch, command flags). Hand-written `IpcInit`/`IpcAccept`/`Ipc_RequestStateDump` in `cpp/samp_meng/ipc.cpp`. Main() restructured with `-dump`/`-ipc`/normal branching.

**Stale socket detection deferred:** PID-based naming (`/tmp/<ns>.<pid>.sock`) makes collisions nearly impossible. The existing `unlink`-before-`bind` in `BindUnix` handles stale sockets from crashed processes at the same PID. Connect probe adds complexity without value for PID-based paths.

**Lessons learned:**

7. *cstring is NOT null-terminated.* `ch_elems` cannot be passed to C functions that expect `const char*`. Use `Zeroterm()` to null-terminate, or pass `(ch_elems, ch_n)` pairs. This was a latent bug in the original IpcCleanup.

8. *dispatch `textcall:Y` vs `read:Y`.* The `textcall` flag generates `DispatchText(ctx, line)` with a context parameter — needed for IPC dispatch where the handler needs the connection. `read:Y` generates `ReadStrptrMaybe(str, buf)` without context. The original acr_nav dispatch used `textcall:Y`; samp_meng initially had `read:Y` which generated the wrong dispatch functions.

9. *`dispctx` record required.* The dispatch context type (`dmmeta.dispctx`) must be declared for `textcall` dispatches. Without it, `DispatchText` is not generated even when `textcall:Y` is set.

**Key files:** `cpp/amc/ipc.cpp` (generator), `cpp/acr_nav/ipc.cpp`, `cpp/samp_meng/ipc.cpp`, `cpp/samp_meng/samp_meng.cpp`.

### Phase 3.3 -- TUI + IPC integration

Enable IPC on a running TUI instance so an external process can connect and poll StateDump while the user navigates interactively. This is A's side of the "live debugger demo."

Replace the TUI's blocking `ReadKeyName()` loop (`cpp/acr_nav/main.cpp:754-766`) with epoll-driven `MainLoop()`. Same pattern as Phase 3.1: register stdin as a non-blocking FIohook, register the IPC listen socket alongside it, render after each key dispatch.

**Harder than Phase 3.1 because:**

- **Raw terminal mode.** `ReadKeyName()` parses multi-byte ANSI escape sequences (arrows, function keys). A single keypress may arrive as 1-6 bytes across multiple reads. The callback must accumulate bytes and parse sequences, not assume one read = one key.
- **SIGWINCH.** Currently the signal handler sets a flag and interrupts blocking `read()` (SA_RESTART disabled). With epoll, SIGWINCH must either use `signalfd` or set the flag and let the next epoll wakeup handle it.
- **Rendering cycle.** Must render after each key dispatch, not on a timer. The callback processes a key, then writes the rendered frame to stdout.

**Scope:** Modify the TUI branch in `Main()`. When `-ipc` is set, use `MainLoop()` instead of the blocking `ReadKeyName()` loop. Add a `TuiKeyCallback()` that reads raw bytes, parses escape sequences, calls `ProcessKey()`, and renders. When `-ipc` is not set, the existing blocking loop is preserved (simpler, correct, no need to change).

**Result:** `acr_nav -ipc` in TUI mode — user navigates normally, external process connects to socket and polls state. The program being inspected doesn't know or care that it's being watched.

### Phase 3.4 -- Live data viewmode (B's side)

A new viewmode in acr_nav that connects to another running instance's IPC socket and displays its pool state. This is B's side of the "live debugger demo."

B connects to A's socket (`/tmp/acr_nav.<pid>.sock`), sends `RequestStateDump`, parses the ssim response, and displays it in the navigator. B already knows A's types — they're in the shared schema (dmmeta). No dynamic schema loading needed.

**What's needed:**

- A `-connect:<socket_path>` command-line flag (or discover via `ls /tmp/acr_nav.*.sock`)
- A poll timer that sends `RequestStateDump` at ~100ms intervals
- A viewmode that shows live pool records instead of schema structure
- Parse incoming ssim tuples into displayable rows, grouped by ctype

**What's free:** B loads all of dmmeta at startup. `acr_nav.FPanel`, `acr_nav.FNavstack`, `acr_nav.FCtype` are known types. The ssim format is the same format acr_nav already parses. The navigator already knows how to display ctypes and their fields.

**Result:** Two terminal panes. Left: acr_nav A, user navigating. Right: acr_nav B, showing A's FNavstack growing, FPanel.sel updating, FFilter changing — live. The schema is the instrumentation.

### Phase 4 -- Input interface (`dmmeta.rtquery`)

Declare program commands in ssimfiles, generate dispatch scaffolding. Currently the IPC dispatch uses direct `RequestStateDump_ReadStrptrMaybe()` -- adding a `dispatch_msg` record is a silent no-op. Refactor to dispatch `call:Y` so dispatch_msg records drive code generation. Investigate whether `dmmeta.dispatch` already covers this (10 dispatch records exist; `atf_amc.Ssimfiles` with `read:Y` generates exactly the "try-deserialize-call-handler" loop).

### Phase 5 -- Second namespace: prove it generalizes

Enable `nsdump` + `nsipc` on a second program to validate the generators work beyond acr_nav. acr_nav was the development vehicle — Phase 5 proves the capability is generic.

**Candidate:** A long-running server or stateful program where pool inspection has real diagnostic value (see "Where the value is highest" section). `samp_meng` is a minimal option for smoke-testing the generators, but a program with meaningful runtime state is more convincing.

**What "prove it works" means:**

- Add `dmmeta.nsdump` and `dmmeta.nsipc` records for the target namespace
- Add the IPC structural records (FIpcconn ctype, FDb fields, fstep, dispatch) following the acr_nav pattern
- Write `IpcInit()` and `IpcAccept()` (extern functions, ~25 lines each — same as `cpp/acr_nav/ipc.cpp`)
- `amc` regenerates — StateDump and IPC infrastructure appear in the target's `cpp/gen/`
- Build, run with `-ipc`, connect from another process, get StateDump response
- Document any generator changes needed (ideally zero — if the generator needs acr_nav-specific fixes, it's not generic yet)

**Success criteria:** Adding state dump + IPC to a new program requires only ssim records + two small extern functions. Zero generator changes. The capability is a schema-level opt-in, not a code-level integration project.

### Beyond Phase 5 -- Possible extensions

Ideas that don't have phases yet. Each would need a use case before committing:

- **Richer query filtering.** Current filter is a ctype-name regex. Field-level predicates (e.g., "show FConnection where state:idle") would let agents ask sharper questions. But this edges toward a runtime query interpreter — an anti-pattern per the vision. The consumer-side filtering via grep/agent is the intended model. Only revisit if consumer-side proves insufficient in practice.
- **`dmmeta.rtquery` as semantic marker.** Even without Phase 4's full dispatch generation, a simple marker record saying "this program accepts RequestStateDump over IPC" would let agents discover capabilities via `acr dmmeta.rtquery ns:acr_nav` without reading source code.
- **Differential dumps.** Send only what changed since the last poll, not the full state. Useful when B polls A at high frequency and the state is large. Would require sequence numbers or checksums per pool. Premature until Phase 3.4 reveals whether full dumps are actually a bottleneck.
- **IpcInit/IpcAccept generation.** Currently these are extern (hand-written) because they call `lib_netio` which lives behind the gen/hand-written header boundary. If the generator could emit them directly (e.g., by adding `lib_netio` includes to the generated header), the two extern functions disappear and onboarding a new namespace becomes pure ssim records — zero hand-written code.

## Generator mechanics (verified, updated 2026-04-03)

95 generators exist in `data/amcdb/gen.ssim`. Two added for state dump: `gen:ns_state_dump` (Phase 1+2) and `gen:ns_ipc` (Phase 3), both `perns:Y`. Adding a new generator requires: 1 ssimfile record, 1 C++ function, 1 targsrc record. Uniform `void()` contract. Generator ordering matters: `ns_ipc` must come after `ns_state_dump` (depends on StateDump existing) and before `ns_funcindex` (which prints function bodies).

Key constraint: `perns:Y` generators run after all field/ctype processing. They cannot create synthetic structural records (ctypes, fields, fbuf, fstep). All structure must be declared in ssimfiles. Generators only fill in function bodies. Only `perns:N` generators like `gen:dispenum` run early enough to create synthetic ctypes.
