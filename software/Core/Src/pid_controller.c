//
// Created by Maarten on 29-2-2024.
#include "pid_controller.h"
#include "pid.h"
#include "set_point.h"
#include "mixer.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal.h"


void get_set_points( drone_pids_t *drone_pids){

        set_point_calculation(&drone_pids->set_points.yaw_set_point, drone_pids->stick_outputs.yaw_stick_output,(float) 0.2 , (float) 0.2);
        set_point_calculation(&drone_pids->set_points.roll_set_point, drone_pids->stick_outputs.roll_stick_output,(float) 0.2,(float) 0.2);
        set_point_calculation(&drone_pids->set_points.pitch_set_point, drone_pids->stick_outputs.pitch_stick_output,(float) 0.2,(float) 0.2);


}

void set_pids(drone_pids_t *drone_pids) {


    drone_pids->set_points.yaw_set_point.imu_output = 375;
    drone_pids->set_points.roll_set_point.imu_output = 182;
    drone_pids->set_points.pitch_set_point.imu_output = 82;
    //LOGD("Set points (in set_pids before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    // LOGD("Measurement (in set_pids before): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(100);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);

    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);

    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);
    //LOGD("Set points (in set_pids after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids after): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(10);
}

void set_pids_change_test(drone_pids_t *drone_pids){
    drone_pids->set_points.yaw_set_point.imu_output = 380;
    drone_pids->set_points.roll_set_point.imu_output = 185;
    drone_pids->set_points.pitch_set_point.imu_output = 85;
    //LOGD("Set points (in set_pids_change_test before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids_change_test before): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    //HAL_Delay(10);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);

    //LOGD("Set points (in set_pids_change_test after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids_change_test after): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    //HAL_Delay(10);
}
void set_pids_change_test2(drone_pids_t *drone_pids){
    drone_pids->set_points.yaw_set_point.imu_output = 400;
    drone_pids->set_points.roll_set_point.imu_output = 190;
    drone_pids->set_points.pitch_set_point.imu_output = 90;
    //LOGD("Set points (in set_pids_change_test before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids_change_test before): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    //HAL_Delay(10);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);

    //LOGD("Set points (in set_pids_change_test after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    //LOGD("Measurement (in set_pids_change_test after): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
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

    limits_init(&drone_pids.set_points.yaw_set_point, drone_pids.pids.yaw_pid.limits.min_output, drone_pids.pids.yaw_pid.limits.max_output);
    limits_init(&drone_pids.set_points.roll_set_point, drone_pids.pids.roll_pid.limits.min_output, drone_pids.pids.roll_pid.limits.max_output);
    limits_init(&drone_pids.set_points.pitch_set_point, drone_pids.pids.pitch_pid.limits.min_output, drone_pids.pids.pitch_pid.limits.max_output);

    drone_pids.stick_outputs.yaw_stick_output = 435;
    drone_pids.stick_outputs.roll_stick_output = 192;
    drone_pids.stick_outputs.pitch_stick_output = 95;
    //HAL_Delay(100);
    get_set_points(&drone_pids);

    set_pids(&drone_pids);

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



    set_pids_change_test(&drone_pids);
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
    set_pids_change_test2(&drone_pids);
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