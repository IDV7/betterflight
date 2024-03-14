//
// Created by Maarten on 5-3-2024.
//
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"

void limits_init(set_point_t *set_point, float min_imu_output, float max_imu_output){
    set_point->stick_limits.min_stick_output = 1000;
    set_point->stick_limits.max_stick_output = 2000;
    set_point->imu_limits.min_imu_output = min_imu_output;
    set_point->imu_limits.max_imu_output = max_imu_output;
}

void set_point_calculation(set_point_t *set_point, float stick_output){
    set_point->stick_output = stick_output;
    LOGD("Stick output: %f", set_point->stick_output);
    LOGD("Stick limits: %f min, %f max", set_point->stick_limits.min_stick_output, set_point->stick_limits.max_stick_output);
    //limits the stick output
    if(stick_output> 1515 && stick_output < set_point->stick_limits.max_stick_output){
        set_point->set_point = (stick_output-1515) * set_point->imu_limits.max_imu_output;
        LOGD("Set point: %f", set_point->set_point);

    }
    if(stick_output < 1485 && stick_output > set_point->stick_limits.min_stick_output){
        set_point->set_point = (stick_output-1485) * set_point->imu_limits.max_imu_output;
        LOGD("Set point: %f", set_point->set_point);
    }


}