//
// Created by Maarten on 5-3-2024.
//

#ifndef BETTERFLIGHT_SET_POINT_H
#define BETTERFLIGHT_SET_POINT_H

#include "stdint.h"

typedef struct{
    int16_t min_stick_output;
    int16_t max_stick_output;
}stick_limits_t;

typedef struct {
    int16_t min_imu_output;
    int16_t max_imu_output;
}imu_limits_t;

typedef struct {
    int16_t stick_output;
    stick_limits_t stick_limits;
    imu_limits_t imu_limits;
    int16_t set_point;
    int16_t imu_output;
} set_point_t;

void limits_init(set_point_t *set_point, int16_t min_imu_output, int16_t max_imu_output);
void set_point_calculation(set_point_t *set_point, int16_t stick_output);

#endif //BETTERFLIGHT_SET_POINT_H
