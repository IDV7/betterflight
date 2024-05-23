#include "pid_controller.h"
#include "pid.h"
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal.h"


void get_set_points( drone_pids_t *drone_pids){

        set_point_calculation(&drone_pids->set_p.yaw_set_point, drone_pids->stick_outputs.yaw_stick_output, (float) 0.2 , (float) 0.2);
        set_point_calculation(&drone_pids->set_p.roll_set_point, drone_pids->stick_outputs.roll_stick_output, (float) 0.2, (float) 0.2);
        set_point_calculation(&drone_pids->set_p.pitch_set_point, drone_pids->stick_outputs.pitch_stick_output, (float) 0.2, (float) 0.2);


}

void set_pids(drone_pids_t *drone_pids, float x_roll_imu_output, float y_pitch_imu_output, float z_yaw_imu_output) {


    drone_pids->set_p.yaw_set_point.imu_val = (int16_t)z_yaw_imu_output;
    drone_pids->set_p.roll_set_point.imu_val = (int16_t)x_roll_imu_output;
    drone_pids->set_p.pitch_set_point.imu_val = (int16_t)y_pitch_imu_output;
    //LOGD("Set points (in set_pids before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_p.yaw_set_point.set_point, drone_pids->set_p.roll_set_point.set_point, drone_pids->set_p.pitch_set_point.set_point);
    // LOGD("Measurement (in set_pids before): %d jaw, %d roll, %d pitch", drone_pids->set_p.yaw_set_point.imu_val, drone_pids->set_p.roll_set_point.imu_val, drone_pids->set_p.pitch_set_point.imu_val);
    //HAL_Delay(100);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_p.yaw_set_point.set_point, drone_pids->set_p.yaw_set_point.imu_val);

    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_p.roll_set_point.set_point, drone_pids->set_p.roll_set_point.imu_val);

    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_p.pitch_set_point.set_point, drone_pids->set_p.pitch_set_point.imu_val);
    //LOGD("Set points (in set_pids after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_p.yaw_set_point.set_point, drone_pids->set_p.roll_set_point.set_point, drone_pids->set_p.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids after): %d jaw, %d roll, %d pitch", drone_pids->set_p.yaw_set_point.imu_val, drone_pids->set_p.roll_set_point.imu_val, drone_pids->set_p.pitch_set_point.imu_val);
    //HAL_Delay(10);
}



void test_pid_controller(void){
    mixer_handle_t mixer_h;
    drone_pids_t drone_pids;

    mixer_h.percentages.roll = 0.7;
    mixer_h.percentages.pitch = 0.7;
    mixer_h.percentages.yaw = 0.4;
    mixer_h.percentages.throttle = 1.0;



    pid_controller_init(&drone_pids.pids.yaw_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);
    pid_controller_init(&drone_pids.pids.roll_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);
    pid_controller_init(&drone_pids.pids.pitch_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);

    limits_init(&drone_pids.set_p.yaw_set_point, drone_pids.pids.yaw_pid.limits.min_output, drone_pids.pids.yaw_pid.limits.max_output);
    limits_init(&drone_pids.set_p.roll_set_point, drone_pids.pids.roll_pid.limits.min_output, drone_pids.pids.roll_pid.limits.max_output);
    limits_init(&drone_pids.set_p.pitch_set_point, drone_pids.pids.pitch_pid.limits.min_output, drone_pids.pids.pitch_pid.limits.max_output);


    drone_pids.stick_outputs.yaw_stick_output = 435;
    drone_pids.stick_outputs.roll_stick_output = 192;
    drone_pids.stick_outputs.pitch_stick_output = 95;
    //HAL_Delay(100);
    get_set_points(&drone_pids);

    set_pids(&drone_pids,180,82,375);

    LOGD("PID output: %d roll, %d pitch, %d yaw\r\n", drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output, drone_pids.pids.yaw_pid.output);

    mixer_h.pid.roll_pid = drone_pids.pids.roll_pid;
    mixer_h.pid.pitch_pid = drone_pids.pids.pitch_pid;
    mixer_h.pid.yaw_pid = drone_pids.pids.yaw_pid;


    mixer_h.input.roll = drone_pids.pids.roll_pid.output;
    mixer_h.input.pitch = drone_pids.pids.pitch_pid.output;
    mixer_h.input.yaw = drone_pids.pids.yaw_pid.output;
    mixer_h.input.throttle = 1800;
    mixing(&mixer_h);


    drone_pids.stick_outputs.yaw_stick_output = 430;
    drone_pids.stick_outputs.roll_stick_output = 185;
    drone_pids.stick_outputs.pitch_stick_output = 90;
    //HAL_Delay(100);
    get_set_points(&drone_pids);


    set_pids(&drone_pids,185,85,380);
    LOGD("PID output: %d jaw, %d roll, %d pitch\r\n", drone_pids.pids.yaw_pid.output, drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output);
    HAL_Delay(25);

    mixer_h.pid.roll_pid = drone_pids.pids.roll_pid;
    mixer_h.pid.pitch_pid = drone_pids.pids.pitch_pid;
    mixer_h.pid.yaw_pid = drone_pids.pids.yaw_pid;


    mixer_h.input.roll = drone_pids.pids.roll_pid.output;
    mixer_h.input.pitch = drone_pids.pids.pitch_pid.output;
    mixer_h.input.yaw = drone_pids.pids.yaw_pid.output;
    mixer_h.input.throttle = 1800;
    mixing(&mixer_h);

    HAL_Delay(25);

    set_pids(&drone_pids,190,90,400);
    LOGD("PID output: %d jaw, %d roll, %d pitch\r\n", drone_pids.pids.yaw_pid.output, drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output);

    mixer_h.pid.roll_pid = drone_pids.pids.roll_pid;
    mixer_h.pid.pitch_pid = drone_pids.pids.pitch_pid;
    mixer_h.pid.yaw_pid = drone_pids.pids.yaw_pid;


    mixer_h.input.roll = drone_pids.pids.roll_pid.output;
    mixer_h.input.pitch = drone_pids.pids.pitch_pid.output;
    mixer_h.input.yaw = drone_pids.pids.yaw_pid.output;
    mixer_h.input.throttle = 1800;
    mixing(&mixer_h);
}
void pid_process(mixer_handle_t *mixer, drone_pids_t *drone_pids){


    mixing(mixer);
}

void pid_init(drone_pids_t *drone_pids){

    pid_controller_init(&drone_pids->pids.yaw_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);
    pid_controller_init(&drone_pids->pids.roll_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);
    pid_controller_init(&drone_pids->pids.pitch_pid, (float).4, (float)0.3, (float)0.3, (float)0.05, -750, 750, -750, 750);

    limits_init(&drone_pids->set_p.yaw_set_point, drone_pids->pids.yaw_pid.limits.min_output, drone_pids->pids.yaw_pid.limits.max_output);
    limits_init(&drone_pids->set_p.roll_set_point, drone_pids->pids.roll_pid.limits.min_output, drone_pids->pids.roll_pid.limits.max_output);
    limits_init(&drone_pids->set_p.pitch_set_point, drone_pids->pids.pitch_pid.limits.min_output, drone_pids->pids.pitch_pid.limits.max_output);

}
