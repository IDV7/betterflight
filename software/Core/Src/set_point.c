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

float set_point_calculation(set_point_t *set_point, float stick_output){
    set_point->stick_output = stick_output;
    set_point->set_point = (stick_output-1500) * set_point->imu_limits.max_imu_output;
    return set_point->set_point;
}