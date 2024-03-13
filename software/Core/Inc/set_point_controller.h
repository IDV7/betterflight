//
// Created by Maarten on 5-3-2024.
//

#ifndef BETTERFLIGHT_SET_POINT_CONTROLLER_H
#define BETTERFLIGHT_SET_POINT_CONTROLLER_H

#include "set_point.h"

void set_points_calculations(float yaw_stick_output, float roll_stick_output, float pitch_stick_output, set_point_t yaw_set_point,
                             set_point_t roll_set_point,set_point_t pitch_set_point);

#endif //BETTERFLIGHT_SET_POINT_CONTROLLER_H
