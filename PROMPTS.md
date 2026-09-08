# Prompt Cookbook - Day 1 Embedded Systems Demo

Ready-to-paste prompts for the elevator controller sample app, organized by
Day 1 agenda topic. Use these alongside `README.md`, which tells you exactly
when to reach for each one.

> Swap file/function names if you rearrange the demo - the prompts assume
> `src/elevator_controller.c` and `src/sensor_utils.c` as written.

## Topic 2 - GitHub Copilot Features

### Code completions

Open `src/elevator_controller.c`, delete the `return 0;` stub inside
`compute_direction`, then type a partial line and pause for ghost text:

```
if (target_floor
```

### Multi-line suggestions

In `state_to_string`, delete `return NULL;` and type:

```
switch (state) {
```

Pause - Copilot should suggest the full `case` block for every
`car_state_t` value. Tab to accept, `Alt+]` / `Alt+[` to cycle alternatives.

### Inline chat (Ctrl+I)

Place the cursor inside `can_open_doors` and open inline chat:

```
Doors must only open when the car is stationary (speed_mm_s == 0) and has
no active fault. Fix this function so it enforces both conditions instead
of always returning true.
```

Then target the harder, multi-branch function `apply_speed_limit`:

```
Replace the stub so the returned speed is tiered by floors_remaining:
3 or more floors -> full requested speed capped at MAX_SPEED_MM_S,
2 floors -> 70% of requested speed, 1 floor -> 40%, 0 floors -> 0.
```

### Slash commands in Chat

With `is_overspeed` selected:

```
/fix the comparison so overspeed trips above MAX_SPEED_MM_S instead of below it
```

With `dispatch_next_floor` selected:

```
/doc
```

With `run_cycle` selected:

```
/tests generate unit tests for run_cycle covering: normal travel, arrival
at the target floor, and an overweight fault
```

### Terminal usage

In the integrated terminal (or Copilot Chat's terminal context):

```
Build and run this project with make, and explain any compiler warnings.
```

```
The build failed - explain the error above and suggest a fix.
```

## Topic 3 - Context and Prompt Engineering

### Chat participants and context

```
@workspace How does sensor_utils.c relate to elevator_controller.c in this project?
```

```
#file:src/sensor_utils.c #file:src/elevator_controller.c
Where would you wire average_weight_samples into run_cycle so overweight
detection uses live sensor samples instead of a single stored value?
```

### Vague -> specific prompt pattern

Vague:

```
Make the dispatch smarter.
```

Specific (use the reusable pattern from the prompt engineering notes):

```
In dispatch_next_floor (src/elevator_controller.c), extend the nearest-floor
logic so that on a tie in distance, the car prefers continuing in its
current direction of travel instead of always picking the lower floor.

Requirements:
- Input: pending_requests array, count, current_floor, current direction
- Output: the chosen floor (int8_t)
- Constraints: no dynamic allocation, keep it embedded-C style
- Relevant context: dispatch_next_floor, compute_direction

First summarize your assumptions, then propose the smallest change.
```

### Anti-pattern to call out live

```
Fix my code.
```

Ask the room what is missing (which file, which function, what "fix" means,
what the current vs. expected behavior is) before Copilot can help well.

## Topic 4 - Coding Agent and PR Review

Use the sample issue in `README.md` ("Sample issue to file") describing the
divide-by-zero in `average_weight_samples`. After assigning it to the
coding agent:

```
Summarize the pull request the coding agent opened. Does the fix guard
against count == 0, and does it match the behavior described in the issue?
```

```
Review this pull request for edge cases the fix might have missed, such as
a NULL samples pointer or a count larger than the actual array.
```

## Topic 5 - GitHub Copilot Spaces

Grounded questions to ask inside a Space that includes this folder plus the
sample issue/PR from Topic 4:

```
Based on the sources in this Space, summarize the known defects in the
elevator controller sample, which are fixed, and which are still open.
```

```
Identify the files and functions most likely to need changes if we added a
fire-service (Phase I recall) mode to this controller. Explain why before
proposing code changes.
```

## Topic 6 - Usage-Based Billing / Model Selection

```
Explain what state_to_string does and why it needs a switch statement.
```

Use a lightweight/auto-selected model for the explanation above, then switch
to a stronger model in Agent mode for the harder tiered-speed change in
`apply_speed_limit` - see README.md Topic 6 for the full talking points.

## Mode Comparison - Ask vs Plan vs Agent

Same task, three chat modes, so the room can see each mode's distinct
behavior side by side. Builds on the fire-service question teed up in
Topic 5.

### Ask mode - understand, no edits

Switch the chat mode picker to **Ask**, then:

```
What would it take to add a fire-service (Phase I recall) mode to this
elevator controller? Identify the affected files/functions and describe
the expected safety behavior, but don't write any code yet.
```

Expected: a grounded explanation only - Ask mode won't touch files even if
the wording implies action.

### Plan mode - design before executing

Switch to **Plan** mode, then:

```
Plan adding a fire-service (Phase I recall) mode to the elevator
controller: when active, the car ignores new floor requests, travels
non-stop to the recall floor (assume floor 1), opens its doors, and
refuses further dispatch until the mode is cleared. Produce a step-by-step
implementation plan across elevator_controller.h/.c (and sensor_utils.c if
needed) before any code is written.
```

Expected: a structured, reviewable checklist/plan with no file edits yet -
contrast with Ask's prose-only answer.

### Agent mode - execute end to end

Switch to **Agent** mode, then:

```
Implement the fire-service (Phase I recall) mode: add a
fire_service_active flag and a FIRE_SERVICE car_state, update run_cycle so
the car ignores new requests and travels non-stop to floor 1, opens its
doors, and blocks dispatch until cleared. Update state_to_string and the
Makefile self-checks accordingly, then build and run the project to
confirm every self-check still passes.
```

Expected: multi-file edits, an autonomous build/self-check loop, and
iteration on failures - contrast with Ask (no edits) and Plan (edits
deferred until you approve and switch modes).

### Debrief prompt (after all three)

```
Compare how Ask, Plan, and Agent mode each handled the fire-service recall
request. What did each produce, and when would you reach for each one on
a real embedded task?
```
