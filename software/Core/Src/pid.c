//
// Created by Maarten on 29-2-2024.
//
#include "pid.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal.h"


void  pid_controller_init(pid_handle_t *pid_h, float Kp, float Ki, float Kd, float T, int16_t min_output, int16_t max_output, int16_t min_input, int16_t max_input)
{
    pid_h->gains.Kp = Kp;
    pid_h->gains.Ki = Ki;
    pid_h->gains.Kd = Kd;

    pid_h->T = T;

    pid_h->limits.min_output = min_output;
    pid_h->limits.max_output = max_output;

    pid_h->limits.min_input = min_input;
    pid_h->limits.max_input = max_input;

    pid_h->integrator = 0;
    pid_h->prev_error = 0;
    pid_h->differentiator = 0;
    pid_h->prev_measurement = 0;
    pid_h->output = 0;
}



void  pid_controller_clear(pid_handle_t *pid_h)
{
    pid_h->integrator = 0;
    pid_h->prev_error = 0;
    pid_h->differentiator = 0;
    pid_h->prev_measurement = 0;
    pid_h->output = 0;
}

int16_t pid_controller_update(pid_handle_t *pid_h, int16_t setp, int16_t measurement){

    int16_t error = setp - measurement;
    LOGD("Error: %d, setp: %d, measurement %d", error, setp, measurement);
    int16_t proportional =  (int16_t )(pid_h->gains.Kp *(float) error); //p[n]


            pid_h->integrator = pid_h->integrator + 0.5f * pid_h->gains.Ki * pid_h->T * ((float)error + pid_h->prev_error);             //i[n]

            pid_h->differentiator = -(2.0f * pid_h->gains.Kd * ( (float)measurement - pid_h->prev_measurement) + (2.0f * pid_h->tau - pid_h->T)*pid_h->differentiator); //d[n]

            pid_h->output = (int16_t) ((float)proportional + (float) pid_h->integrator + (float)pid_h->differentiator); //u[n]

            LOGD("PID output(before limited): %d", pid_h->output);
            //HAL_Delay(500);
            if (pid_h->output > pid_h->limits.max_output){
                pid_h->output = pid_h->limits.max_output;
            }
            else if (pid_h->output < pid_h->limits.min_output){
                pid_h->output = pid_h->limits.min_output;
            }


            pid_h->prev_error = error;
            pid_h->prev_measurement = measurement;

            return pid_h->output;
}


