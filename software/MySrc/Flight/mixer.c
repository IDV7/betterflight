#include "mixer.h"

void mixing(mixer_handle_t *mixer_h, motor_output_t *motor_output){
    //LOGD("Throttle input: %d, roll input  %d, pitch input %d, yaw input %d", mixer_h->input.throttle, mixer_h->input.roll, mixer_h->input.pitch, mixer_h->input.yaw);
    HAL_Delay(10);
    float throttle = ((float)mixer_h->input.throttle)*mixer_h->percentages.throttle;
    float roll = ((float)mixer_h->input.roll)* mixer_h->percentages.roll;
    float pitch = ((float)mixer_h->input.pitch)* mixer_h->percentages.pitch;
    float yaw = ((float)mixer_h->input.yaw)* mixer_h->percentages.yaw;

    uint16_t temp;


    LOGD("Throttle: %f, roll %f, pitch %f, yaw %f", throttle, roll, pitch, yaw);
    HAL_Delay(10);
    motor_output->motor1 = (int16_t)((throttle + roll- pitch - yaw));
    motor_output->motor2 = (int16_t)((throttle - roll - pitch + yaw));
    motor_output->motor3 = (int16_t)((throttle - roll + pitch - yaw));
    motor_output->motor4 = (int16_t)((throttle + roll + pitch + yaw));

        if(motor_output->motor1 > 2000){
            temp = motor_output->motor1 - 2000;
         //   LOGD("Temp1+: %d", temp);
            motor_output->motor1 -= (temp + 50);
            motor_output->motor2 -= (temp + 50);
            motor_output->motor3 -= (temp + 50);
            motor_output->motor4 -= (temp + 50);
        }
        if(motor_output->motor2 > 2000){

            temp = motor_output->motor2 - 2000;
         //   LOGD("TEMP2+: %d", temp);
            motor_output->motor1 -= (temp + 50);
            motor_output->motor2 -= (temp + 50);
            motor_output->motor3 -= (temp + 50);
            motor_output->motor4 -= (temp + 50);
        }
        if(motor_output->motor3 > 2000){
            temp = motor_output->motor3 - 2000;
         //   LOGD("TEMP3+: %d", temp);
            motor_output->motor1 -= (temp + 50);
            motor_output->motor2 -= (temp + 50);
            motor_output->motor3 -= (temp + 50);
            motor_output->motor4 -= (temp + 50);
        }
        if(motor_output->motor4 > 2000){
            temp = motor_output->motor4 - 2000;
         //   LOGD("TEMP4+: %d", temp);
            motor_output->motor1 -= (temp + 50);
            motor_output->motor2 -= (temp + 50);
            motor_output->motor3 -= (temp + 50);
            motor_output->motor4 -= (temp + 50);
        }




        if (motor_output->motor1 < 0) {
            temp = 50 - (motor_output->motor1);
            LOGD("TEMP1: %d", temp);
            //delay(10);
            motor_output->motor1 += temp;
            motor_output->motor2 += (int16_t) motor_output->motor1;
            motor_output->motor3 += (int16_t) motor_output->motor1;
            motor_output->motor4 += (int16_t) motor_output->motor1;

        }
        if (motor_output->motor2 < 0) {
            temp = 50 - (motor_output->motor2);
            LOGD("TEMP2: %d", temp);
            //delay(10);
            motor_output->motor2 += temp;
            motor_output->motor1 += (int16_t) motor_output->motor2;
            motor_output->motor3 += (int16_t) motor_output->motor2;
            motor_output->motor4 += (int16_t) motor_output->motor2;

        }
        if (motor_output->motor3 < 0) {
            temp = 50 - (motor_output->motor3);
            LOGD("TEMP3: %d", temp);
           // delay(10);
            motor_output->motor3 += temp;
            motor_output->motor1 += (int16_t) motor_output->motor3;
            motor_output->motor2 += (int16_t) motor_output->motor3;
            motor_output->motor4 += (int16_t) motor_output->motor3;

        }
        if (motor_output->motor4 < 0) {
            temp = 50 - (motor_output->motor4);
            LOGD("TEMP4: %d", temp);
            //delay(10);
            motor_output->motor4 += temp;
            motor_output->motor1 += (int16_t) motor_output->motor4;
            motor_output->motor2 += (int16_t) motor_output->motor4;
            motor_output->motor3 += (int16_t) motor_output->motor4;

        }
        if(motor_output->motor1 > 2000){
            motor_output->motor1 = 2000;
        }
        if(motor_output->motor2 > 2000){
            motor_output->motor2 = 2000;
        }
        if(motor_output->motor3 > 2000){
            motor_output->motor3 = 2000;
        }
        if(motor_output->motor4 > 2000){
            motor_output->motor4 = 2000;
        }




    LOGD("Motor 1: %d, Motor 2: %d, Motor 3: %d, Motor 4: %d", motor_output->motor1, motor_output->motor2, motor_output->motor3, motor_output->motor4);

/*
    LOGD  ("Motor 1: %d", mixer_h->output.motor1);
    HAL_Delay(10);
    LOGD  ("Motor 2: %d", mixer_h->output.motor2);
    HAL_Delay(10);
    LOGD  ("Motor 3: %d", mixer_h->output.motor3);
    HAL_Delay(10);
    LOGD  ("Motor 4: %d", mixer_h->output.motor4);
    HAL_Delay(10);
    */
}

void init_mixer_percentages(mixer_handle_t *mixer_h){
    mixer_h->percentages.roll = 0.65;
    mixer_h->percentages.pitch = 0.65;
    mixer_h->percentages.yaw = 0.35;
    mixer_h->percentages.throttle = 1.0;
}