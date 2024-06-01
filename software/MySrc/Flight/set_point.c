//
// Created by Maarten on 5-3-2024.
//
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"
#define MAX(a, b) (((a) > (b)) ? (a) : (b));

float power5(float x) {
    return x * x * x * x * x;
}

void limits_init(set_point_handle_t *set_point, int16_t min_imu_output, int16_t max_imu_output){
    set_point->stick_limits.min_stick_output = 1000;
    set_point->stick_limits.max_stick_output = 2000;
    set_point->actual_rates.rc_rates = (float) 0.20; // center sensitivity
    set_point->actual_rates.rates = (float) 0.600; // max rate
    set_point->actual_rates.rc_expo = (float) 0.20; // expo
}

int16_t set_point_calculation(set_point_handle_t *set_point, int16_t stick_output,float rc_commandf, const float rc_commandfabs){

    //LOGD("Stick output: %d", stick_output);

    float expof = set_point->actual_rates.rc_expo / 100.0f;
    expof = rc_commandfabs * (power5(rc_commandf) * expof + rc_commandf * (1 - expof));

    const float centerSensitivity = set_point->actual_rates.rc_rates * 10.0f;
    const float stickMovement = MAX(0, set_point->actual_rates.rates * 10.0f - centerSensitivity);
    const float angleRate = rc_commandf * centerSensitivity + stickMovement * expof;
    //LOGD("set_point %d",(int16_t)(angleRate * (float)stick_output));
    return ((int16_t)(angleRate * (float)stick_output));

}