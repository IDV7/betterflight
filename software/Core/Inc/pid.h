//
// Created by Maarten on 29-2-2024.
//

#ifndef BETTERFLIGHT_PID_H
#define BETTERFLIGHT_PID_H

#include "stdint.h"
typedef struct{
    //gains pid
    float Kp;
    float Ki;
    float Kd;
}pid_gain_t;

typedef struct {
    // PID limits
    float min_output;
    float max_output;                       //limits of the motor output
    float min_input;
    float max_input;                        //limits of the input sticks
}pid_limits_t;

typedef struct{

    pid_gain_t gains;               //pid gains

    float tau;                      //low pass filter time constant

    pid_limits_t limits;            //pid limits

    float T;                        // sample time  (s)

    float integrator;
    float prev_error;               // needed for integrator
    float differentiator;
    float prev_measurement;         // needed for differentiator



    float output;



}pid_handle_t;

void  pid_controller_init(pid_handle_t *pid_h, float Kp, float Ki, float Kd, float T, float min_output, float max_output, float min_input, float max_input);      //initializes the pid controller
void  pid_controller_clear(pid_handle_t *pid_h);        //clears the calculations
float pid_controller_update(pid_handle_t *pid_h, float setp, float measurement);         //updates the pid controller with user input



#endif //BETTERFLIGHT_PID_H
