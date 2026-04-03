---
name: agent-test-demo
description: Interactive demo of AI-driven testing. Plants a real bug in acr_nav, launches a blind subagent to find it through headless mode. Use to showcase agent-testable programs.
---

# Agent-Test Demo

Interactive demonstration of AI-driven testing of acr_nav through its headless protocol. The headline act: plant a real, previously-fixed bug, rebuild, then launch a subagent that finds it blind.

This is an orchestrator's playbook. Subagents receive only `references/protocol.md` (symlink to `../agent-test/references/protocol.md`; requires symlink-capable git checkouts).

## Prerequisite

Verify headless mode and build work:

```bash
printf 'acr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null | head -1
# Must produce an acr_nav.Screen line

abt acr_nav 2>&1 | tail -1
# Must show successful build
```

If either fails, tell the user to run `ai` first to bootstrap the build environment.

## Step 1: Opening

Keep this to 3-4 sentences. The audience knows OpenACR but not acr_nav or headless mode. Be direct, no filler.

Output something like: "acr_nav is a TUI that browses the OpenACR schema — ctypes, fields, cross-references. It has a headless mode: structured ssim commands on stdin, structured state (Screen, PanelState, VisibleField) on stdout. An AI agent can drive it, freeze state, read typed records, reason, and decide the next action — the program-as-database idea, running in a loop. What follows is not scripted — I'll be making real decisions based on what I observe."

Then say: "Let me show you what the protocol looks like."

## Step 1b: Live Protocol Demo

Walk through 3 round-trips with the headless protocol. For each one: say what you're about to send and why, run it, then explain what came back and what it means. Narrate like you're thinking aloud, not reading a spec.

### Round-trip 1: Navigate + Summary

Before running, explain: "I'm going to send two commands. `Navigate` tells acr_nav to select a ctype — like clicking on it in the TUI. `Summary` asks for a compact state snapshot. This is stateful — the program remembers where I navigated, just like a real user session."

```bash
printf 'acr_nav.Navigate  ctype:dmmeta.Field\nacr_nav.Summary\n' | acr_nav -headless 2>/dev/null
```

After output, explain the key idea: "This is the program frozen mid-execution. The Screen record is the top-level state — what mode we're in, what's selected, how many ctypes match the filter. The two PanelState records are the left and right panels — each one tells me what's selected and how many items are in the list. 3 records, ~400 bytes. That's the entire TUI state as structured data."

Emphasize: "The program is an in-memory database. Every run builds tables in memory — ctypes, fields, namespaces. The Screen and PanelState records are projections of those tables, just like a SQL view. The headless protocol lets an agent read those projections as ssim tuples instead of rendering them as pixels."

### Round-trip 2: Screenshot with VisibleField

Explain: "Summary is compact but shallow. Screenshot gives me everything — including what's visible in each panel. Let me ask for the full picture."

```bash
printf 'acr_nav.Navigate  ctype:dmmeta.Field\nacr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null
```

After output, highlight 2-3 VisibleField records. Explain: "Each VisibleField is a row in the right panel. `arg` is the type, `reftype:Pkey` means it's a foreign key, `navigable:Y` means I can follow it — like clicking a hyperlink in the schema. The agent doesn't guess which fields are references — it reads a typed record that says so."

### Round-trip 3: Follow a reference

Explain: "Now I'll do something a human would do — follow one of those navigable fields to see where it leads. This is `SendKey key:Enter`, which pushes the current state onto a stack and navigates to the target ctype. The program remembers where I was, so I can come back."

```bash
printf 'acr_nav.Navigate  ctype:dmmeta.Field\nacr_nav.SendKey  key:Enter\nacr_nav.Summary\n' | acr_nav -headless 2>/dev/null
```

After output, point out: navstack_depth increased (the program saved my previous location), the selected ctype changed (I followed the reference), and the breadcrumb shows the navigation path. "This is the observe-reason-act loop: I read the state, saw a navigable field, decided to follow it, and now I can read the new state. Each step transforms the program's in-memory database into the next state — exactly like stepping through a debugger, but with structured data instead of a screen."

### Wrap up

"Every amc-generated program has typed pools like this in memory. Right now, only acr_nav exposes them through a headless protocol. But the structure is there in every program — it's just not accessible yet. That's what the runtime state dump generator would change."

Then move to the interactive menu.

## Step 2: Interactive Menu

Use AskUserQuestion:

**Question:** "What would you like to see?"

**Option 1: Bug Hunt (Recommended)**
Label: "Bug Hunt"
Description: "I'll introduce a real, previously-discovered bug into acr_nav's source code, rebuild it, then launch an AI agent that knows nothing about the bug. Watch it explore the program through structured state inspection and find the defect."

**Option 2: Guided Exploration**
Label: "Guided Exploration"
Description: "I'll drive acr_nav through its headless protocol, navigating the OpenACR schema step by step. At each point I'll show you the structured output and explain my reasoning — a live demonstration of the observe-reason-act loop."

## Step 3a: Bug Hunt

### 3a.1: Select bug category

Read `references/known_bugs.md`. Present the 3 bug categories to the user via AskUserQuestion. Use the **category label** and **category description** from each bug entry. Mark Bug A as recommended (it's the most dramatic — high severity, data loss). Do NOT reveal what the actual bug is — just the category.

### 3a.2: Plant the bug

1. Read the selected bug's "What to change" section from `references/known_bugs.md`
2. Read the source file to find the exact code
3. Make the edit using the Edit tool
4. Show the user a brief summary: "I've introduced a defect in [area]. Here's what I changed:" followed by a concise description of the code change (e.g., "removed state save/restore logic from filter cancel"). Do NOT reveal the expected symptom.
5. Run `git diff` to show the change

### 3a.3: Rebuild and hide evidence

```bash
abt -build -install acr_nav
```

If build fails: revert with `git checkout <file>`, explain the failure, abort gracefully.

After successful build, hide the source change so the subagent can't find it via `git diff` or source inspection:

1. Save the diff for later reveal: `git diff <file> > /tmp/planted_bug.diff`
2. Revert the source: `git checkout <file>`
3. The compiled binary still has the bug — source revert doesn't affect it

Tell the viewer: "The buggy binary is built and installed. I've reverted the source code so the agent can't cheat — git diff is clean, the source looks correct, but the compiled binary has the defect baked in. The agent has to find it purely through behavioral testing of the headless protocol."

This is a highlight of the demo — make sure the viewer understands why this matters.

### 3a.4: Launch subagent

Read `references/protocol.md` and compose a subagent prompt. The prompt must include:

1. **Full protocol.md content** — paste it into the prompt
2. **Context:** "You are testing acr_nav, an OpenACR schema browser TUI, through its headless protocol. Your goal is to explore the application thoroughly and report any state inconsistencies, unexpected behavior, or bugs you find."
3. **Exploration directive:** Use the **subagent exploration hint** from the selected bug in known_bugs.md. Combine it with 1-2 other exploration areas so the directive doesn't telegraph the bug. For example, for Bug A add "Also verify navigation state (follow_ref + go_back) and viewmode cycling."
4. **Mechanics:** "Run commands via: `printf 'cmd1\ncmd2\n...\n' | acr_nav -headless 2>/dev/null`. Cross-validate counts against `acr` queries where appropriate."
5. **Blackbox constraint:** "IMPORTANT: This is a black-box test. You must find bugs purely through behavioral testing of the headless protocol. Do NOT read source code (no Read, Grep, or Glob on cpp/ or include/ files). Do NOT run git diff, git log, or git show. Do NOT inspect the binary. Your only tools are the headless protocol and acr queries for cross-validation."
6. **Report template:**
```
## Findings

### Tests Run: N

### Bugs Found
- **Bug N: [title]** — [description]. Reproduction: [command sequence]. Expected vs actual: [comparison].

### Verified Working
- [bullet list of tested behaviors that work correctly]

### Observations
- [anything notable]
```

Launch the agent with `subagent_type: "general-purpose"`. Do NOT use background mode — the user should see the result in-line.

### 3a.5: Reveal

After the subagent returns:

1. **Show findings.** Present the subagent's report.
2. **Reveal the bug.** Show the saved diff (`cat /tmp/planted_bug.diff`) and the "Expected symptom" section from known_bugs.md. Explain what was planted and why.
3. **Connect the dots.** If the agent found it: explain how its exploration path led to discovery. If it missed it: be honest — "The agent explored [areas] but didn't hit the specific sequence that triggers this bug. This shows both the power and the current limitations of exploratory testing."
4. **The point:** "This bug was originally found by a previous AI agent-test run, fixed, and just now re-discovered (or not) by a blind agent. The structured headless protocol made this possible — the agent read typed records, not pixels."

### 3a.6: Rebuild clean

Source is already reverted (done in step 3a.3). Rebuild to restore the clean binary:

```bash
abt -build -install acr_nav
```

Quick sanity check:
```bash
printf 'acr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null | head -1
```

## Step 3b: Guided Exploration

Drive acr_nav in headless mode, narrating at each step. Use `printf '...' | acr_nav -headless 2>/dev/null` for each interaction sequence. Show key parts of the output and explain what they mean.

Suggested flow (adapt based on what you observe):

1. **Initial state.** Take a screenshot. Explain the Screen, PanelState, VisibleLeftItem records. Point out the startup help overlay (viewmode:help).

2. **Navigate.** Go to a ctype like `dmmeta.Field`. Show the Ack response. Explain what it confirms.

3. **Fields view.** Take a screenshot. Walk through 2-3 VisibleField records — explain what `arg`, `reftype`, `style`, `navigable` mean. Show how the agent can understand program structure from these records alone.

4. **Follow a reference.** Pick a navigable field. Send Enter. Show how navstack_depth increases, breadcrumb updates, and the new ctype appears. "I just followed a typed relationship in the schema — like clicking a foreign key."

5. **Cross-validate.** Compare a count from the headless output (e.g., n_items in PanelState) against an `acr` query. Show they match. "The structured output is trustworthy — it matches the source of truth."

6. **Filter.** Apply a filter. Show how n_sel_ctype changes. "The agent can narrow focus programmatically."

7. **Go back.** Pop the navstack. Show that previous state is fully restored — viewmode, selection, filter, scroll position. "This is the program-as-database idea: state transforms are reversible."

At each step, show the command you're sending and the key output fields. Keep narration concise — focus on what's surprising or non-obvious.

## Step 4: Closing

Output this conclusion (adapt wording, hit all points):

**What you just saw:** An AI agent treating a running program as a structured database — freezing state, reading typed records, reasoning about what it found, deciding what to do next.

**What makes this work:** acr_nav's headless protocol. Structured commands in, structured state out. No screen scraping, no regex parsing, no brittle heuristics. The same ssimfile format the entire OpenACR system is built on.

**Where this goes:** Right now, headless mode is hand-built for acr_nav. But every amc-generated program already has typed pools in its FDb. If amc could generate a state dump function — iterate all reachable records, emit them as ssim tuples with regex filtering — then any OpenACR program would get this capability for free. That's the runtime state dump generator: a built-in pretty printer for the whole program, from the schema. The path from here to there is a single amc generator.
