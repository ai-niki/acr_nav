# acr_nav

A TUI for exploring OpenACR schema.

## Demo

https://github.com/user-attachments/assets/a044cf31-97d4-401a-9070-c2f6d4aa1975

## Origin

I was learning the OpenACR schema through `acr` queries. After a while I wanted something I could navigate interactively — follow a field reference, go back, filter without constructing queries. So I built this.

## What It Does

- **Two-panel layout** — ctypes on the left, fields for the selected ctype on the right
- **Navigation** — `Enter` follows a field reference, `Backspace` pops the stack; breadcrumb bar shows where you are; vim keys (j/k/h/l/g/G) work throughout
- **Filter** — live filter; `Tab` cycles target: ctype, field, arg, reftype, or all
- **Viewmodes** — fields, reverse xrefs, ssimfile preview (navigable FK columns), codegen (syntax highlighted), access path graph, namespace deps, per-field dep detail, field detail drilldown
- **Namespace tree** — collapsible groups with record counts; `?` opens keybinding help
- **Graph view** — interactive access path diagram, reftype-colored fields, bidirectional (forward fields + reverse xrefs)
- **Headless protocol** — stdin/stdout ssim interface for agent-driven use (see below)

## Headless Protocol

acr_nav accepts a `-headless` flag that replaces the TUI with a structured protocol: commands in as ssim tuples, state out as ssim tuples.

```
acr_nav.Navigate  ctype:amc.FField
acr_nav.Summary
→ acr_nav.Screen       ctype:amc.FField  navmode:browse  ...
→ acr_nav.PanelState   side:right  n_item:42  sel:0  ...
```

Commands: `Navigate`, `SetFilter`, `SetView`, `GoBack`, `Summary`, `Screenshot`, `SendKey`, `SetTermSize`.

`Summary` emits ~400 bytes of typed state. `Screenshot` emits full panel contents as `VisibleField` / `VisibleLeftItem` records. No screen scraping — the program exposes its internal state as ssim projections.

## Agent Testing

Two Claude Code skills ship with the repo:

**`/agent-test`** — exploratory testing via parallel subagents. Each agent drives acr_nav through the headless protocol, probing combinations across orthogonal state axes (navmode × viewmode × filter × navstack depth). Findings get captured as `atf_comp` regression tests.

**`/agent-test-demo`** — plants a real previously-fixed bug, rebuilds, then launches a blind subagent to find it through the protocol alone. No source inspection.

Component tests live in `test/atf_comp/acr_nav.*` and run with `normalize comp`.

## Runtime State Inspection

https://github.com/user-attachments/assets/48735f43-f5bd-4c75-a954-98c7a65de735

amc generates a `StateDump()` function that walks every pool in `_db` and emits records as ssim. One opt-in record per namespace:

```
dmmeta.nsdump  ns:acr_nav    # generates StateDump()
dmmeta.nsipc   ns:acr_nav    # generates IPC socket listener
```

**Dump on exit** — print state matching a regex and exit:

```bash
acr_nav -dump:"acr_nav.FPanel"    # dump FPanel records
acr_nav -dump:".*"                # dump everything
```

**IPC socket** — query a running instance without restarting it:

```bash
acr_nav -ipc                      # TUI + IPC socket at /tmp/acr_nav.<pid>.sock
echo 'acr_nav.RequestStateDump  filter:"acr_nav.FPanel"' \
  | socat - UNIX-CONNECT:/tmp/acr_nav.$(pgrep acr_nav).sock
```

**Live inspect** — one acr_nav watching another in real time:

```bash
# Terminal A                      # Terminal B
acr_nav -ipc                      acr_nav -connect /tmp/acr_nav.<pid>.sock
```

B polls A every 100ms. Left panel shows live pools with record counts. Enter follows references between pools. `s` toggles static (finput) pool visibility. FDb singleton gets a vertical detail card.

The generators (`ns_state_dump`, `ns_ipc`) are namespace-generic — any amc program can opt in with two ssim records and two hand-written functions (`IpcInit`, `IpcAccept`).

**Claude Code as consumer.** The `/statedump` skill connects to a running instance and reads its pool state. During development, Claude can launch a program with `-ipc`, inspect its runtime state, diagnose issues, and verify fixes — all without stopping the process. The dump is ssim, so Claude parses it the same way it reads any other OpenACR data.

Design notes: [`.claude/plans/statedump_vision.md`](.claude/plans/statedump_vision.md)

## Getting Started

```bash
ai                              # bootstrap build
abt -build -install acr_nav     # build acr_nav
acr_nav                         # run
```

---

Built on [OpenACR](README_OPENACR.md).
