# Day 1 Demo: GitHub Copilot for Embedded Systems

A guided, hands-on demo that covers every Day 1 agenda topic against one
realistic C sample app: a simplified **elevator car controller** (state
machine, motor speed limiting, door safety interlocks, and floor dispatch).
Embedded-C style is used throughout - fixed-width types, no dynamic
allocation, no exceptions - so it behaves like real controller firmware
while still being buildable and testable on a laptop.

Use this folder alongside `agenda.md` (Day 1 row) at the repository root.
Ready-to-paste prompts for every step live in [`PROMPTS.md`](PROMPTS.md).

## Folder contents

```txt
day1-embedded-demo/
  README.md                 <- this file: the full demo script
  PROMPTS.md                <- ready-to-paste prompts, grouped by topic
  Makefile                  <- build/run targets used throughout the demo
  src/
    elevator_controller.h   <- shared types and function declarations
    elevator_controller.c   <- state machine, dispatch, self-checks (main)
    sensor_utils.h
    sensor_utils.c          <- sensor helpers, incl. the Topic 4 defect
  .github/
    workflows/ci.yml        <- build + self-check gate for PRs
    ISSUE_TEMPLATE/bug_report.md
```

## Prerequisites

- VS Code (or another supported IDE) with GitHub Copilot enabled and signed in.
- A C compiler and `make` available in the terminal (gcc/clang on Linux/macOS,
  or a Dev Container/WSL on Windows). If none is available, the code walk
  can still be presented from the editor without running `make run`.
- This folder pushed to a GitHub repository, for the Topic 4 and 5 sections
  that use Issues, pull requests, and Copilot Spaces.

Sanity check before presenting:

```powershell
make -C day1-embedded-demo run
```

Every self-check prints `FAIL` at this point - that is expected. Each topic
below fixes a subset of them; by the end of Topic 2 all checks print `PASS`.

---

## Topic 1 - Introduction to GitHub Copilot (30 min)

**Objective:** confirm setup and frame why an elevator controller is a good
stand-in for embedded firmware work.

**Talking points**

- Position Copilot: an AI pair programmer available as completions, chat,
  and an autonomous coding agent, across supported IDEs (VS Code, Visual
  Studio, JetBrains, Xcode, Eclipse) and the CLI.
- Embedded caveat to set expectations: Copilot cannot flash hardware or run
  a debugger on a target board, but it is very effective on the C/C++ logic,
  build scripts, and tests that surround that firmware - exactly what this
  sample app represents.
- Walk the audience through activation/setup: Copilot status icon in the
  IDE status bar, sign-in state, and where completions can be toggled on/off
  per language.

**Demo steps**

1. Open this folder in VS Code. Open `src/elevator_controller.c`.
2. Point out the Copilot icon in the status bar; confirm it shows "Ready".
3. Briefly scroll the file and narrate the domain: a car moves between
   floors, must not exceed rated speed or weight, and must not open its
   doors while moving - real safety interlocks, simplified for a demo.

**Checkpoint:** everyone can see Copilot is active in the editor, and
understands the sample app models a physical elevator car.

---

## Topic 2 - GitHub Copilot Features (60 min)

**Objective:** demo completions, multi-line suggestions, inline chat, slash
commands, and terminal usage - all against real stubs and bugs in the file.

**Talking points**

- Completions predict the next few tokens; multi-line suggestions can
  produce a whole block (e.g. a `switch` statement) when there is enough
  surrounding context.
- Inline chat (`Ctrl+I`) is best for a small, local, well-scoped edit
  without leaving the editor.
- Panel chat slash commands (`/fix`, `/doc`, `/tests`, `/explain`) apply a
  known operation to selected code.
- Copilot in the terminal can explain commands, build errors, and suggest
  next steps.

**Demo steps**

1. **Code completions** - in `compute_direction`, delete `return 0;` and
   retype the guided line from `PROMPTS.md` ("Code completions"). Accept
   with `Tab`.
2. Repeat for `is_overweight` (single-line completion).
3. **Multi-line suggestions** - in `state_to_string`, delete `return NULL;`
   and type `switch (state) {`; pause for the full suggestion.
4. **Inline chat** - place the cursor in `can_open_doors`, press `Ctrl+I`,
   and paste the door-safety prompt from `PROMPTS.md`.
5. **Inline chat, harder** - repeat on `apply_speed_limit` with the tiered
   speed-limit prompt. Discuss how a multi-branch requirement still works
   well with inline chat when the prompt spells out every tier.
6. **Slash commands** - select `is_overspeed` and run `/fix`; select
   `dispatch_next_floor` and run `/doc`; select `run_cycle` and run
   `/tests`.
7. **Terminal usage** - in the integrated terminal, run:

   ```powershell
   make -C day1-embedded-demo run
   ```

   All nine self-checks should now print `PASS`, ending with a sample
   control-loop summary. If anything still fails, ask Copilot Chat to
   explain the failing check using the terminal-usage prompt in
   `PROMPTS.md`.

**Checkpoint:** `make run` prints `ALL PASS` and a boxed cycle summary.

---

## Topic 3 - Context and Prompt Engineering (60 min)

**Objective:** show how chat participants, file context, and prompt
structure change response quality - using the two-file split between
`elevator_controller.c` and `sensor_utils.c`.

**Talking points** (see `copilot-prompt-engineering-speaker-notes.md` at
the repo root for the full script)

- State the outcome first, then inputs/outputs/constraints/error behavior.
- Point Copilot at the right code: open/select relevant files, use
  `@workspace` or `#file` instead of relying on implicit context.
- Iterate deliberately; start a new thread for a new task instead of
  letting stale history bias the answer.

**Demo steps**

1. Ask the cross-file context question from `PROMPTS.md`
   (`@workspace ... sensor_utils.c ... elevator_controller.c`).
2. Ask the `#file` question that proposes wiring
   `average_weight_samples` into `run_cycle`.
3. Run the **vague -> specific** pair from `PROMPTS.md` against
   `dispatch_next_floor`. Compare the two answers live.
4. Read the anti-pattern prompt ("Fix my code.") aloud and ask the room
   what context is missing before Copilot could act on it well.

**Checkpoint:** the room can articulate the difference between a vague and
a well-scoped prompt using the two live answers just produced.

---

## Topic 4 - Coding Agent and PR Review (40 min)

**Objective:** file a real defect as a GitHub issue, assign it to the
Copilot coding agent, and review the resulting pull request.

**Talking points**

- The coding agent works from an assigned issue: it creates a branch,
  implements a fix, opens a PR, and iterates on review comments.
- `.github/workflows/ci.yml` in this folder builds and runs the self-checks
  on every push/PR, so the agent's PR gets a real pass/fail signal.

**Sample issue to file** (uses the `.github/ISSUE_TEMPLATE/bug_report.md`
template in this folder):

```txt
Title: [bug]: average_weight_samples divides by zero when no samples exist

Summary: average_weight_samples() in src/sensor_utils.c divides the running
total by `count` with no check for count == 0, which is undefined behavior
on startup before any weight samples have been collected.

Steps to reproduce:
1. Call average_weight_samples(samples, 0) before any samples are recorded.

Expected vs actual:
- Expected: return 0.0 (or another safe default) when count == 0.
- Actual: divides by zero.

Relevant files: src/sensor_utils.c
```

**Demo steps**

1. Push this folder to a GitHub repository (or use one already prepared).
2. File the issue above using the repository's "New issue" flow.
3. Assign the issue to the Copilot coding agent (via the assignee picker,
   or `@copilot` in a comment, depending on your plan/UI).
4. While the agent works, discuss the CI workflow that will validate its PR.
5. Once the PR opens, walk the diff, then use the PR-review prompts from
   `PROMPTS.md` to ask Copilot to summarize the change and flag missed
   edge cases (NULL pointer, oversized `count`).
6. Merge (or simulate merging) after review.

**Checkpoint:** a merged (or ready-to-merge) PR that guards
`average_weight_samples` against `count == 0`, with a green CI run.

---

## Topic 5 - GitHub Copilot Spaces (20 min)

**Objective:** show how a Space preserves project context across sessions.
Full script: `copilot-spaces-speaker-notes.md` at the repo root.

**Demo steps**

1. Create a Space named "Elevator Controller Firmware".
2. Add this folder's repository, the issue filed in Topic 4, its resulting
   PR, and a short note: "Sample embedded controller for Day 1 Copilot
   training."
3. Ask the two grounded questions from `PROMPTS.md` (Topic 5 section).
4. Emphasize that the next presenter/engineer can reuse this Space instead
   of re-explaining the project from scratch.

**Checkpoint:** a Space exists with the repo, issue, and PR attached, and
answers a grounded question correctly using only those sources.

---

## Topic 6 - Usage-Based Billing (30 min)

**Objective:** connect model/mode choice to AI credit usage, using the
demo work already completed today as concrete examples.

**Talking points** (full detail in `copilot-ai-credits-notes.md` at the
repo root)

- Code completions and next-edit suggestions are unlimited on paid plans;
  they do not consume AI credits. Everything done in Topic 2's completions
  steps was effectively free.
- Chat requests (Ask, Edit, Agent, coding agent sessions) draw from a
  pooled or per-plan AI credit allowance, priced by model and token volume.
- Auto model selection routes simple asks to lightweight/cheaper models and
  reserves stronger models for harder reasoning, with a cost discount on
  supported surfaces.

**Demo steps**

1. Re-run the simple explanation prompt from `PROMPTS.md`
   ("Explain what state_to_string does...") with Auto/lightweight model
   selection and note it is a cheap, fast answer.
2. Contrast with the harder `apply_speed_limit` tiered-logic change from
   Topic 2: a multi-branch implementation task is exactly the kind of work
   worth a stronger model in Agent mode.
3. Recap the cost-saving habits from the notes file: scope prompts to the
   relevant file/function, prefer Ask mode for learning, use Plan mode
   before large changes, and start a new thread when history is stale.

**Checkpoint:** the room can name at least two concrete choices from
today's demo (mode, model, prompt scope) that affect AI credit usage.

---

## Wrap-up

By the end of Day 1, the sample app should build cleanly, all nine
self-checks in `elevator_controller.c` should pass, the `average_weight_samples`
defect should be fixed via a reviewed PR, and a Copilot Space should exist
capturing the whole exercise - a working, if small, tour of the full Copilot
workflow on an embedded-style codebase.
