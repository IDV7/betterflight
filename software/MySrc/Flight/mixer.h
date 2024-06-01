/*
 * mixing is called from main loop during the flight control cycle
 * The percentages can be changed to compute for a non-symmetrical quadcopter and are multiplied with the pid values
 *The mixing calculates all the motor outputs based on all the pid output values and the throttle value
 */



#ifndef BETTERFLIGHT_MIXER_H
#define BETTERFLIGHT_MIXER_H

#include <stdio.h>
#include "stdint.h"
#include "pid.h"
#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal.h"
#include "common_structs.h"

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
    mixer_input_t input;
    mixer_percentages_t percentages;
    motor_output_t output;

}mixer_handle_t;

/*
 * mixing is called from main loop during the flight control cycle
 * the motor values are limited to the motor_high_limit and motor_low_limit
 */
void mixing(mixer_handle_t *mixer_h, motor_output_t *motor_output);
void init_mixer_percentages(mixer_handle_t *mixer_h);
#endif //BETTERFLIGHT_MIXER_H
