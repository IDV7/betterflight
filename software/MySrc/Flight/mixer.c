#include "mixer.h"

void mixing(mixer_handle_t *mixer_h, motor_output_t *motor_output){
    int16_t motor_low_limit = 200;
    int16_t motor_high_limit = 1600;

    float throttle = ((float)mixer_h->input.throttle)*mixer_h->percentages.throttle;
    float roll = ((float)mixer_h->input.roll)* mixer_h->percentages.roll;
    float pitch = ((float)mixer_h->input.pitch)* mixer_h->percentages.pitch;
    float yaw = ((float)mixer_h->input.yaw)* mixer_h->percentages.yaw;

    uint16_t temp;
/*
 * [4]-------[2]
 * |          |
 * |          |
 * |          |
 * [3]-------[1]
 */


    motor_output->motor4 = (int16_t)((throttle + pitch + roll - yaw)); //must be motor 4 was motor 1
    motor_output->motor3 = (int16_t)((throttle - pitch + roll + yaw)); //must be motor 3 was motor 2
    motor_output->motor1 = (int16_t)((throttle - pitch - roll - yaw)); //must be motor 1 was motor 3
    motor_output->motor2 = (int16_t)((throttle + pitch - roll + yaw)); //must be motor 2 was motor 4

        if(motor_output->motor1 > motor_high_limit){
            temp = motor_output->motor1 - motor_high_limit;

            motor_output->motor1 -= temp ;
            motor_output->motor2 -= temp;
            motor_output->motor3 -= temp ;
            motor_output->motor4 -= temp ;
        }
        if(motor_output->motor2 > motor_high_limit){

            temp = motor_output->motor2 - motor_high_limit;

            motor_output->motor1 -= temp ;
            motor_output->motor2 -= temp ;
            motor_output->motor3 -= temp ;
            motor_output->motor4 -= temp ;
        }
        if(motor_output->motor3 > motor_high_limit){
            temp = motor_output->motor3 - motor_high_limit;

            motor_output->motor1 -= temp ;
            motor_output->motor2 -= temp ;
            motor_output->motor3 -= temp ;
            motor_output->motor4 -= temp ;
        }
        if(motor_output->motor4 > motor_high_limit){
            temp = motor_output->motor4 - motor_high_limit;

            motor_output->motor1 -= temp ;
            motor_output->motor2 -= temp ;
            motor_output->motor3 -= temp ;
            motor_output->motor4 -= temp ;
        }




        if (motor_output->motor1 < motor_low_limit) {
            temp = motor_low_limit - (motor_output->motor1);

            motor_output->motor1 += temp;
            motor_output->motor2 += temp;
            motor_output->motor3 += temp;
            motor_output->motor4 += temp;

        }
        if (motor_output->motor2 < motor_low_limit) {
            temp = motor_low_limit - (motor_output->motor2);

            motor_output->motor2 += temp;
            motor_output->motor1 += temp;
            motor_output->motor3 += temp;
            motor_output->motor4 += temp;

        }
        if (motor_output->motor3 < motor_low_limit) {
            temp = motor_low_limit - (motor_output->motor3);

            motor_output->motor3 += temp;
            motor_output->motor1 += temp;
            motor_output->motor2 += temp;
            motor_output->motor4 += temp;

        }
        if (motor_output->motor4 < motor_low_limit) {
            temp = motor_low_limit - (motor_output->motor4);

            motor_output->motor4 += temp;
            motor_output->motor1 += temp;
            motor_output->motor2 += temp;
            motor_output->motor3 += temp;

        }

    if(motor_output->motor1 > motor_high_limit){
        motor_output->motor1 = motor_high_limit;
    }
    if(motor_output->motor2 > motor_high_limit){
        motor_output->motor2 = motor_high_limit;
    }
    if(motor_output->motor3 > motor_high_limit){
        motor_output->motor3 = motor_high_limit;
    }
    if(motor_output->motor4 > motor_high_limit){
        motor_output->motor4 = motor_high_limit;
    }




    if(motor_output->motor1 < motor_low_limit){
        motor_output->motor1 = motor_low_limit;
    }
    if(motor_output->motor2 < motor_low_limit){
        motor_output->motor2 = motor_low_limit;
    }
    if(motor_output->motor3 < motor_low_limit){
        motor_output->motor3 = motor_low_limit;
    }
    if(motor_output->motor4 < motor_low_limit){
        motor_output->motor4 = motor_low_limit;
    }


   // LOGD("Motor 1: %d, Motor 2: %d, Motor 3: %d, Motor 4: %d", motor_output->motor1, motor_output->motor2, motor_output->motor3, motor_output->motor4);


}

void init_mixer_percentages(mixer_handle_t *mixer_h){
    mixer_h->percentages.roll = 1;
    mixer_h->percentages.pitch = 1;
    mixer_h->percentages.yaw = 1;
    mixer_h->percentages.throttle = 1;
}