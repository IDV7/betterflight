/*
This file contains the implementation of a PID (Proportional-Integral-Derivative) controller.
The pid_controller_init  initializes the PID controller with specified gains, time step, input/output limits, and other parameters.
The pid_controller_update calculates the PID output based on the setpoint and measurement, it ensures the output stays within specified limits before returning it.
*/
 #ifndef BETTERFLIGHT_PID_H
#define BETTERFLIGHT_PID_H

#include "stdint.h"
#include "common_structs.h"

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

/*
 *  pid_controller_update function calculates the PID control output as follows:
 *  Computes the error difference between setpoint and measurement.
 *  Calculates the proportional term using Kp.
 *  Calculates the integrator term using Ki, T, current and previous errors, with clamping to prevent windup.
 *  Calculates the differentiator term using the derivative gain (Kd), the time constant (tau), and the change in measurement, incorporating a low-pass filter.
 *  Adds the proportional, integral, and derivative terms to get the control output.
 *  Clamps the control output within the specified limits before returning it.
 *  Updates the previous error and measurement for the next iteration.
 */
int16_t pid_controller_update(pid_handle_t *pid_h, int16_t *pid, int16_t setp, int16_t measurement);



#endif //BETTERFLIGHT_PID_H
