//
// Created by Maarten on 5-3-2024.
//

#ifndef BETTERFLIGHT_SET_POINT_H
#define BETTERFLIGHT_SET_POINT_H

#include "stdint.h"

typedef struct{
    float min_stick_output;
    float max_stick_output;
}stick_limits_t;

typedef struct {
    float min_imu_output;
    float max_imu_output;
}imu_limits_t;

typedef struct {
    float stick_output;
    stick_limits_t stick_limits;
    imu_limits_t imu_limits;
    float  set_point;
} set_point_t;

void limits_init(set_point_t *set_point, float min_imu_output, float max_imu_output);
#endif //BETTERFLIGHT_SET_POINT_H
