#ifndef BETTERFLIGHT_SET_POINT_H
#define BETTERFLIGHT_SET_POINT_H

#include "stdint.h"

typedef struct{
    int16_t min_stick_output;
    int16_t max_stick_output;
}stick_limits_t;

typedef struct{
    float rc_rates;
    float rc_expo;
    float rates;
}actual_rates_t;


typedef struct {
    int16_t stick_output;
    stick_limits_t stick_limits;
    actual_rates_t actual_rates;
    int16_t set_point;
    int16_t imu_val;
} set_point_t;

void limits_init(set_point_t *set_point, int16_t min_imu_output, int16_t max_imu_output);
float set_point_calculation(set_point_t *set_point, int16_t stick_output,float rc_commandf, const float rc_commandfabs);
float power5(float x);
#endif //BETTERFLIGHT_SET_POINT_H
