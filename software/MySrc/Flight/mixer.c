#include "mixer.h"

void mixing(mixer_handle_t *mixer_h){
    int16_t max_roll = mixer_h->pid.roll_pid.limits.max_output;
    int16_t max_pitch = mixer_h->pid.pitch_pid.limits.max_output;
    int16_t max_yaw = mixer_h->pid.yaw_pid.limits.max_output;
    float throttle = ((float)mixer_h->input.throttle)*mixer_h->percentages.throttle;
    float roll = ((float)mixer_h->input.roll)* mixer_h->percentages.roll;
    float pitch = ((float)mixer_h->input.pitch)* mixer_h->percentages.pitch;
    float yaw = ((float)mixer_h->input.yaw)* mixer_h->percentages.yaw;

    LOGD("Throttle: %f, roll %f, pitch %f, yaw %f", throttle, roll, pitch, yaw);
    HAL_Delay(10);
    mixer_h->output.motor1 = (int16_t)((throttle + roll- pitch - yaw));
    mixer_h->output.motor2 = (int16_t)((throttle - roll - pitch + yaw));
    mixer_h->output.motor3 = (int16_t)((throttle - roll + pitch - yaw));
    mixer_h->output.motor4 = (int16_t)((throttle+ roll + pitch + yaw));



    LOGD  ("Motor 1: %d", mixer_h->output.motor1);
    HAL_Delay(10);
    LOGD  ("Motor 2: %d", mixer_h->output.motor2);
    HAL_Delay(10);
    LOGD  ("Motor 3: %d", mixer_h->output.motor3);
    HAL_Delay(10);
    LOGD  ("Motor 4: %d", mixer_h->output.motor4);
    HAL_Delay(10);
}

void init_mixer_percentages(mixer_handle_t *mixer_h){
    mixer_h->percentages.roll = 0.7;
    mixer_h->percentages.pitch = 0.7;
    mixer_h->percentages.yaw = 0.4;
    mixer_h->percentages.throttle = 1.0;
}