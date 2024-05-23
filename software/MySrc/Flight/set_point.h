#ifndef BETTERFLIGHT_SET_POINT_H
#define BETTERFLIGHT_SET_POINT_H

#include "stdint.h"
#include "common_structs.h"


typedef struct{
    float rc_rates;
    float rc_expo;
    float rates;
}actual_rates_t;


typedef struct {
    stick_limits_t stick_limits;
    actual_rates_t actual_rates;
    int16_t imu_val;
    //set_point_t setp;
} set_point_handle_t;

void limits_init(set_point_handle_t *set_point, int16_t min_imu_output, int16_t max_imu_output);
int16_t set_point_calculation(set_point_handle_t *set_point, int16_t stick_value, float rc_commandf, const float rc_commandfabs);
float power5(float x);
#endif //BETTERFLIGHT_SET_POINT_H
