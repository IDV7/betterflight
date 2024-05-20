//
// Created by Maarten on 3-5-2024.
//

#ifndef BETTERFLIGHT_MIXER_H
#define BETTERFLIGHT_MIXER_H
#include "stdint.h"
#include "pid.h"
#include "log.h"
#include <stdio.h>
#include "misc.h"
#include "stm32f7xx_hal.h"

typedef struct {
    int16_t motor1;
    int16_t motor2;
    int16_t motor3;
    int16_t motor4;
}motor_output_t;


typedef struct {
    float roll;
    float pitch;
    float yaw;
    float throttle;
}mixer_percentages_t;

typedef struct{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    int16_t throttle;
}mixer_input_t;

typedef struct{
    pid_handle_t roll_pid;
    pid_handle_t pitch_pid;
    pid_handle_t yaw_pid;
}mixer_pid_t;

typedef struct{
    mixer_input_t input;
    mixer_percentages_t percentages;
    motor_output_t output;
    mixer_pid_t pid;
}mixer_handle_t;


void mixing(mixer_handle_t *mixer_h);

#endif //BETTERFLIGHT_MIXER_H
