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

## The Bigger Idea

Every amc-generated program has typed pools in `_db`. acr_nav exposes its state through a hand-written headless protocol. But amc already knows every pool via `gen_detectinst()` — it could generate a `StateDump()` function for any opted-in program automatically.

One new record:

```
dmmeta.nsdump  ns:acr_nav
```

→ amc emits `acr_nav::StateDump(out, filter)` that walks every Lary/Inlary pool, emits a census (ctype name + count) and record-by-record dumps for ctypes with print support. Triggered via `-dump` flag or `acr_nav.StateDump` headless command.

Any amc program becomes agent-inspectable at runtime — one generator, one opt-in record per namespace.

- Vision: [`.claude/plans/statedump_vision.md`](.claude/plans/statedump_vision.md)
- MVP plan: [`.claude/plans/statedump_plan.md`](.claude/plans/statedump_plan.md)

## Getting Started

```bash
ai                              # bootstrap build
abt -build -install acr_nav     # build acr_nav
acr_nav                         # run
```

---

Built on [OpenACR](README_OPENACR.md).
