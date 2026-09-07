// ============================================================
//  Day 1 Embedded Systems Demo - Elevator Car Controller (sim)
//  Build & run: make run        (see ../README.md for the full script)
//
//  This is a desktop-testable stand-in for firmware that would normally
//  run on a car controller board. It uses fixed-width types, no dynamic
//  allocation, and a polled state machine - typical embedded-C style -
//  so every Day 1 Copilot feature can be demoed against a realistic file.
// ============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elevator_controller.h"

// ---- Code completion target -----------------------------------
// Return -1 if target is below current, 1 if above, 0 if equal.
int8_t compute_direction(int8_t current_floor, int8_t target_floor) {
    return 0; // code completion completes this
}

// ---- Code completion target -----------------------------------
// A car is overweight when car_weight_kg exceeds MAX_CAR_WEIGHT_KG.
bool is_overweight(float car_weight_kg) {
    return false; // code completion completes this
}

// ---- / commands target: /fix -----------------------------------
// Trip the overspeed fault when speed exceeds the rated contract speed.
// BUG (fix with /fix): the comparison is inverted, so overspeed never trips.
bool is_overspeed(float speed_mm_s) {
    return speed_mm_s < MAX_SPEED_MM_S;
}

// ---- Code completions target (multi-line) -----------------------
// Return a human-readable name for each car_state_t value, e.g. "MOVING_UP".
// Type the `switch (state) {` line and pause for the multi-line suggestion.
const char *state_to_string(car_state_t state) {
    return NULL; // code completion completes this with a switch statement
}

// ---- Inline chat target (Ctrl+I) ---------------------------------
// Doors must only open when the car is stationary (speed_mm_s == 0) and
// has no active fault. BUG: this always returns true, so doors could
// open while the car is still moving. Harden it with inline chat.
bool can_open_doors(const elevator_t *car) {
    return true;
}

// ---- Inline chat target (Ctrl+I) - harder, multi-branch ----------
// Apply a tiered speed limit based on how many floors remain before the
// target floor, so the car decelerates on approach:
//   >=3 floors remaining -> full speed, capped at MAX_SPEED_MM_S
//   2 floors remaining   -> 70% of requested speed
//   1 floor remaining    -> 40% of requested speed
//   0 floors remaining   -> 0 (must be stopped for door leveling)
// Stub currently returns the requested speed unchanged - build the tiers
// with inline chat (Ctrl+I).
float apply_speed_limit(float requested_speed_mm_s, int8_t floors_remaining) {
    return requested_speed_mm_s;
}

// ---- / commands target: /doc (undocumented on purpose) -----------
int8_t dispatch_next_floor(const int8_t *pending_requests, uint8_t count, int8_t current_floor) {
    if (count == 0) return current_floor;
    int8_t best = pending_requests[0];
    int8_t best_distance = (int8_t)abs((int)current_floor - (int)best);
    for (uint8_t i = 1; i < count; i++) {
        int8_t distance = (int8_t)abs((int)current_floor - (int)pending_requests[i]);
        if (distance < best_distance) {
            best = pending_requests[i];
            best_distance = distance;
        }
    }
    return best;
}

// ---- Ask / #symbol / /tests target -------------------------------
// Composes the functions above into one control-loop tick.
void run_cycle(elevator_t *car) {
    car->fault_active = is_overweight(car->car_weight_kg) || is_overspeed(car->speed_mm_s);
    if (car->fault_active) {
        car->state = STATE_EMERGENCY_STOP;
        car->speed_mm_s = 0.0f;
        return;
    }

    int8_t direction = compute_direction(car->current_floor, car->target_floor);
    if (direction == 0) {
        car->speed_mm_s = 0.0f;
        car->state = can_open_doors(car) ? STATE_DOOR_OPEN : STATE_EMERGENCY_STOP;
        return;
    }

    int8_t floors_remaining = (int8_t)abs((int)car->target_floor - (int)car->current_floor);
    car->state = (direction > 0) ? STATE_MOVING_UP : STATE_MOVING_DOWN;
    car->speed_mm_s = apply_speed_limit(MAX_SPEED_MM_S, floors_remaining);
}

static void print_cycle_summary(const elevator_t *car) {
    printf("+------------------------------------------+\n");
    printf("| Floor %2d -> %2d   State: %-13s |\n", car->current_floor, car->target_floor,
           state_to_string(car->state) ? state_to_string(car->state) : "UNKNOWN");
    printf("| Speed: %6.1f mm/s   Weight: %6.1f kg   |\n", car->speed_mm_s, car->car_weight_kg);
    printf("| Fault: %-3s                                |\n", car->fault_active ? "YES" : "no");
    printf("+------------------------------------------+\n");
}

static bool _check(const char *name, bool ok) {
    printf("%-18s: %s\n", name, ok ? "PASS" : "FAIL");
    return ok;
}

int main(void) {
    printf("=== Self-checks ===\n");

    bool c1 = compute_direction(2, 5) == 1 && compute_direction(9, 4) == -1
           && compute_direction(3, 3) == 0;

    bool c2 = is_overweight(700.0f) == true && is_overweight(500.0f) == false;

    // After /fix, overspeed should trip above MAX_SPEED_MM_S, not below it.
    bool c3 = is_overspeed(3000.0f) == true && is_overspeed(2000.0f) == false;

    bool c4 = state_to_string(STATE_MOVING_UP) != NULL
           && strcmp(state_to_string(STATE_MOVING_UP), "MOVING_UP") == 0
           && strcmp(state_to_string(STATE_IDLE), "IDLE") == 0;

    // After the inline-chat fix, a stopped/fault-free car may open its doors...
    elevator_t stopped = { 5, 5, STATE_DOOR_OPENING, 0.0f, 400.0f, false };
    // ...but a moving car must not.
    elevator_t moving = { 5, 8, STATE_MOVING_UP, 1200.0f, 400.0f, false };
    bool c5 = can_open_doors(&stopped) == true && can_open_doors(&moving) == false;

    // After the harder inline-chat edit, the speed tiers should match:
    //   >=3 floors -> full speed (capped), 2 -> 70%, 1 -> 40%, 0 -> stopped.
    bool c6 = apply_speed_limit(3000.0f, 5) == MAX_SPEED_MM_S
           && apply_speed_limit(2000.0f, 2) == 1400.0f
           && apply_speed_limit(2000.0f, 1) == 800.0f
           && apply_speed_limit(500.0f, 0)  == 0.0f;

    int8_t pending[] = { 5, 2, 9 };
    bool c7 = dispatch_next_floor(pending, 3, 4) == 5;

    // run_cycle composes direction + speed tiers + fault checks (needs c1, c3, c6).
    elevator_t enroute = { 2, 6, STATE_IDLE, 0.0f, 500.0f, false };
    run_cycle(&enroute);
    bool c8 = enroute.state == STATE_MOVING_UP && enroute.speed_mm_s == MAX_SPEED_MM_S;

    // run_cycle also composes can_open_doors when the car has arrived (needs c1, c5).
    elevator_t arrived = { 7, 7, STATE_MOVING_UP, 0.0f, 400.0f, false };
    run_cycle(&arrived);
    bool c9 = arrived.state == STATE_DOOR_OPEN;

    bool all = _check("compute_direction", c1) & _check("is_overweight", c2)
             & _check("is_overspeed", c3)       & _check("state_to_string", c4)
             & _check("can_open_doors", c5)     & _check("apply_speed_limit", c6)
             & _check("dispatch_next_floor", c7)
             & _check("run_cycle (moving)", c8) & _check("run_cycle (arrived)", c9);

    if (all) {
        printf("\nALL PASS - sample control-loop tick:\n\n");
        print_cycle_summary(&enroute);
    } else {
        printf("\nSOME FAILED - finish the steps in README.md with Copilot, then re-run.\n");
    }
    return all ? 0 : 1;
}
