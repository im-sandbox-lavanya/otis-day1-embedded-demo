// ============================================================
//  Day 1 sample app - Sensor Utilities
//  Used to demo: code completions, /doc, and the Coding Agent
//  + PR Review flow (see README.md "Topic 4").
// ============================================================

#include "sensor_utils.h"

// ---- Code completion target ---------------------------------
// Return the arithmetic mean of `count` values in `samples`.
float average_weight_samples(const float *samples, uint8_t count) {
    float total = 0.0f;
    for (uint8_t i = 0; i < count; i++) {
        total += samples[i];
    }
    return total / count; // BUG (Coding Agent demo): divides by zero when count == 0
}

// ---- Code completion target ---------------------------------
// A floor sensor is noisy. Only accept `raw_floor` as the new stable
// reading once it has matched the previous raw reading 3 times in a row.
// Otherwise keep reporting `last_stable_floor`. Update *match_count as you go.
int8_t debounce_floor_sensor(int8_t raw_floor, int8_t last_stable_floor, uint8_t *match_count) {
    return last_stable_floor; // code completion completes this
}

// ---- /doc target (undocumented on purpose) -------------------
float clamp_speed(float speed, float min_speed, float max_speed) {
    if (speed < min_speed) return min_speed;
    if (speed > max_speed) return max_speed;
    return speed;
}
