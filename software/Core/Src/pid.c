//
// Created by Maarten on 29-2-2024.
//
#include "pid.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"



void  pid_controller_init(pid_handle_t *pid_h, float Kp, float Ki, float Kd, float T, float min_output, float max_output, float min_input, float max_input)
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

float pid_controller_update(pid_handle_t *pid_h, float setp, float measurement){

            float error = setp - measurement;

            float proportional = pid_h->gains.Kp * error; //p[n]


            pid_h->integrator = pid_h->integrator + 0.5f * pid_h->gains.Ki * pid_h->T * (error + pid_h->prev_error);             //i[n]

            if (pid_h->integrator > pid_h->limits.max_input){
                pid_h->integrator = pid_h->limits.max_input;
            }
            else if (pid_h->integrator < pid_h->limits.min_input){
                        pid_h->integrator = pid_h->limits.min_input;
            }


            pid_h->differentiator = -(2.0f * pid_h->gains.Kd * (measurement - pid_h->prev_measurement) + (2.0f * pid_h->tau - pid_h->T)*pid_h->differentiator); //d[n]


            pid_h->output = proportional + pid_h->integrator + pid_h->differentiator; //u[n]

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


