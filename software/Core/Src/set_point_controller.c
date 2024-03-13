//
// Created by Maarten on 5-3-2024.
//

#include "set_point_controller.h"
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"

void set_point_calculations(float yaw_stick_output, float roll_stick_output, float pitch_stick_output, set_point_t yaw_set_point,
                            set_point_t roll_set_point,set_point_t pitch_set_point){

    set_point_calculation(&yaw_set_point, yaw_stick_output);
    set_point_calculation(&roll_set_point, roll_stick_output);
    set_point_calculation(&pitch_set_point, pitch_stick_output);
}