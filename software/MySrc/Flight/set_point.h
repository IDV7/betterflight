/*
 * set_point_calculation function calculates the setpoint for the PID controller based on the stick value
 * The function uses the actual rates and stick limits to calculate the setpoint.
 * rc_rates: center sensitivity of the stick (smaller adjustments around center, bigger adjustments at the end)
 * rc_expo: expo
 * rates: max rate
 */
#ifndef BETTERFLIGHT_SET_POINT_H
#define BETTERFLIGHT_SET_POINT_H

#include "stdint.h"
#include "common_structs.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b));

typedef struct{
    float rc_rates;
    float rc_expo;
    float rates;
}actual_rates_t;


typedef struct {
    stick_limits_t stick_limits;
    actual_rates_t actual_rates;
    int16_t imu_val;
} set_point_handle_t;

void limits_init(set_point_handle_t *set_point, int16_t min_imu_output, int16_t max_imu_output);
int16_t set_point_calculation(set_point_handle_t *set_point, int16_t stick_value, float rc_commandf, const float rc_commandfabs);
float power5(float x);
#endif //BETTERFLIGHT_SET_POINT_H
