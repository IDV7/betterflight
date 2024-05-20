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
    int16_t min_output;
    int16_t max_output;                       //limits of the motor output
    int16_t min_input;
    int16_t max_input;                        //limits of the input sticks
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



    int16_t output;



}pid_handle_t;

void  pid_controller_init(pid_handle_t *pid_h, float Kp, float Ki, float Kd, float T, int16_t min_output, int16_t max_output, int16_t min_input, int16_t max_input);      //initializes the pid controller
void  pid_controller_clear(pid_handle_t *pid_h);        //clears the calculations
int16_t pid_controller_update(pid_handle_t *pid_h, int16_t setp, int16_t measurement);         //updates the pid controller with user input



#endif //BETTERFLIGHT_PID_H
