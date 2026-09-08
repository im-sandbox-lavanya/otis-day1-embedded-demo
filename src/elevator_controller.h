#ifndef ELEVATOR_CONTROLLER_H
#define ELEVATOR_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#define MIN_FLOOR                 1
#define MAX_FLOOR                 20
#define MAX_CAR_WEIGHT_KG       630.0f   /* rated load: 8-person passenger car */
#define MAX_SPEED_MM_S         2500.0f   /* rated contract speed              */

typedef enum {
    STATE_IDLE = 0,
    STATE_MOVING_UP,
    STATE_MOVING_DOWN,
    STATE_DOOR_OPENING,
    STATE_DOOR_OPEN,
    STATE_DOOR_CLOSING,
    STATE_EMERGENCY_STOP
} car_state_t;

typedef struct {
    int8_t      current_floor;
    int8_t      target_floor;
    car_state_t state;
    float       speed_mm_s;
    float       car_weight_kg;
    bool        fault_active;
    bool        obstruction_detected;
} elevator_t;

int8_t      compute_direction(int8_t current_floor, int8_t target_floor);
bool        is_overweight(float car_weight_kg);
bool        is_overspeed(float speed_mm_s);
const char *state_to_string(car_state_t state);
bool        can_open_doors(const elevator_t *car);
bool        is_door_obstructed(const elevator_t *car);
float       apply_speed_limit(float requested_speed_mm_s, int8_t floors_remaining);
int8_t      dispatch_next_floor(const int8_t *pending_requests, uint8_t count, int8_t current_floor);
void        run_cycle(elevator_t *car);

#endif /* ELEVATOR_CONTROLLER_H */
