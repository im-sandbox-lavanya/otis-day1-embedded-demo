#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <stdint.h>

float average_weight_samples(const float *samples, uint8_t count);
int8_t debounce_floor_sensor(int8_t raw_floor, int8_t last_stable_floor, uint8_t *match_count);
float clamp_speed(float speed, float min_speed, float max_speed);

#endif /* SENSOR_UTILS_H */
