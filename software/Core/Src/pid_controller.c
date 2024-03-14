//
// Created by Maarten on 29-2-2024.
#include "pid_controller.h"
#include "pid.h"
#include "set_point.h"

#include <stdio.h>

#include "log.h"
#include "misc.h"


void get_set_points( drone_pids_t *drone_pids){
        set_point_calculation(&drone_pids->set_points.yaw_set_point, drone_pids->stick_outputs.yaw_stick_output);
        set_point_calculation(&drone_pids->set_points.roll_set_point, drone_pids->stick_outputs.roll_stick_output);
        set_point_calculation(&drone_pids->set_points.pitch_set_point, drone_pids->stick_outputs.pitch_stick_output);
        LOGD("Stick outputs (in get_set_points): %f yaw, %f roll , %f pitch", drone_pids->stick_outputs.yaw_stick_output, drone_pids->stick_outputs.roll_stick_output, drone_pids->stick_outputs.pitch_stick_output);
    LOGD("Set points (in get_set_points): %f jaw, %f roll, %f pitch", drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.pitch_set_point.set_point);
}

void set_pids(drone_pids_t *drone_pids) {
    pid_controller_update(&drone_pids->pids.yaw_pid, drone_pids->set_points.yaw_set_point.set_point, drone_pids->set_points.yaw_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.roll_pid, drone_pids->set_points.roll_set_point.set_point, drone_pids->set_points.roll_set_point.imu_output);
    pid_controller_update(&drone_pids->pids.pitch_pid, drone_pids->set_points.pitch_set_point.set_point, drone_pids->set_points.pitch_set_point.imu_output);
    LOGD("pids (in set_pids): %f yaw, %f roll, %f pitch", drone_pids->pids.yaw_pid.output, drone_pids->pids.roll_pid.output, drone_pids->pids.pitch_pid.output);
}

void test_pid_controller(void){
    drone_pids_t drone_pids;

    pid_controller_init(&drone_pids.pids.yaw_pid, 40, 85, 20, 0.1, 0, 150, 1000, 2000);
    pid_controller_init(&drone_pids.pids.roll_pid, 40, 85, 20, 0.1, 0, 150, 1000, 2000);
    pid_controller_init(&drone_pids.pids.pitch_pid, 40, 85, 20, 0.1, 0, 150, 1000, 2000);

    drone_pids.stick_outputs.yaw_stick_output = 1700;
    drone_pids.stick_outputs.roll_stick_output = 1700;
    drone_pids.stick_outputs.pitch_stick_output = 1700;

    get_set_points(&drone_pids);
    LOGD("Set points (in test_pid_controller): %f jaw, %f roll, %f pitch", drone_pids.set_points.yaw_set_point.set_point, drone_pids.set_points.roll_set_point.set_point, drone_pids.set_points.pitch_set_point.set_point);


    set_pids(&drone_pids);

    LOGD("PID output: %f jaw, %f roll, %f pitch", drone_pids.pids.yaw_pid.output, drone_pids.pids.roll_pid.output, drone_pids.pids.pitch_pid.output);
}