//
// Created by Maarten on 29-2-2024.
#include "pid_controller.h"
#include "pid.h"
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal.h"

void get_set_points( drone_pids_t *drone_pids){

        set_point_calculation(&drone_pids->set_points.yaw_set_point, drone_pids->stick_outputs.yaw_stick_output);
        set_point_calculation(&drone_pids->set_points.roll_set_point, drone_pids->stick_outputs.roll_stick_output);
        set_point_calculation(&drone_pids->set_points.pitch_set_point, drone_pids->stick_outputs.pitch_stick_output);


}

void set_pids(drone_pids_t *drone_pids) {


    drone_pids->set_points.yaw_set_point.imu_output = 1090;
    drone_pids->set_points.roll_set_point.imu_output = 1450;
    drone_pids->set_points.pitch_set_point.imu_output = 1590;
    LOGD("Set points (in set_pids before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    LOGD("Measurement (in set_pids before): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(100);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);

    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);

    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);
    LOGD("Set points (in set_pids after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    LOGD("Measurement (in set_pids after): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(10);
}

void set_pids_change_test(drone_pids_t *drone_pids){
    drone_pids->set_points.yaw_set_point.imu_output = 1200;
    drone_pids->set_points.roll_set_point.imu_output = 1400;
    drone_pids->set_points.pitch_set_point.imu_output = 1200;
    LOGD("Set points (in set_pids_change_test before): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    LOGD("Measurement (in set_pids_change_test before): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(10);
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);

    LOGD("Set points (in set_pids_change_test after): %d jaw, %d roll, %d pitch\r\n", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
    LOGD("Measurement (in set_pids_change_test after): %d jaw, %d roll, %d pitch", drone_pids->set_points.yaw_set_point.imu_output, drone_pids->set_points.roll_set_point.imu_output, drone_pids->set_points.pitch_set_point.imu_output);
    HAL_Delay(10);
}

void test_pid_controller(void){
    drone_pids_t drone_pids;

    pid_controller_init(&drone_pids.pids.yaw_pid, 1, 1, 1, 0.1, -1000, 1000, 1000, 2000);
    pid_controller_init(&drone_pids.pids.roll_pid, 1, 1, 1, 0.1, -1000, 1000, 1000, 2000);
    pid_controller_init(&drone_pids.pids.pitch_pid, 1, 1, 1, 0.1, -1000, 1000, 1000, 2000);

    limits_init(&drone_pids.set_points.yaw_set_point, drone_pids.pids.yaw_pid.limits.min_output, drone_pids.pids.yaw_pid.limits.max_output);
    limits_init(&drone_pids.set_points.roll_set_point, drone_pids.pids.roll_pid.limits.min_output, drone_pids.pids.roll_pid.limits.max_output);
    limits_init(&drone_pids.set_points.pitch_set_point, drone_pids.pids.pitch_pid.limits.min_output, drone_pids.pids.pitch_pid.limits.max_output);

    drone_pids.stick_outputs.yaw_stick_output = 1700;
    drone_pids.stick_outputs.roll_stick_output = 1200;
    drone_pids.stick_outputs.pitch_stick_output = 1800;
    //HAL_Delay(100);
    get_set_points(&drone_pids);

    set_pids(&drone_pids);

    LOGD("PID output: %d jaw, %d roll, %d pitch\r\n", drone_pids.pids.yaw_pid.output, drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output);

    drone_pids.stick_outputs.yaw_stick_output = 1700;
    drone_pids.stick_outputs.roll_stick_output = 1200;
    drone_pids.stick_outputs.pitch_stick_output = 1800;
    //HAL_Delay(100);
    get_set_points(&drone_pids);



    set_pids_change_test(&drone_pids);
    LOGD("PID output: %d jaw, %d roll, %d pitch\r\n", drone_pids.pids.yaw_pid.output, drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output);

}