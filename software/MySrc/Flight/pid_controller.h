#include "set_point.h"
#include "pid.h"
#include "mixer.h"
#ifndef BETTERFLIGHT_PID_CONTROLLER_H
#define BETTERFLIGHT_PID_CONTROLLER_H
typedef struct {
set_point_t yaw_set_point;
set_point_t roll_set_point;
set_point_t pitch_set_point;
}set_points_t;

typedef struct{
    int16_t yaw_stick_output;
    int16_t roll_stick_output;
    int16_t pitch_stick_output;
}stick_output_t;

typedef struct{
    pid_handle_t yaw_pid;
    pid_handle_t roll_pid;
    pid_handle_t pitch_pid;
}pids_t;

typedef struct{
    set_points_t set_p;
    stick_output_t stick_outputs;
    pids_t pids;
}drone_pids_t;

void get_set_points(drone_pids_t *drone_pids);
void set_pids(drone_pids_t *drone_pids, float x_imu_output, float y_imu_output, float z_imu_output);
void test_pid_controller(void);
void pid_init(drone_pids_t *drone_pids);
void pid_process(mixer_handle_t *mixer, drone_pids_t *drone_pids);

#endif //BETTERFLIGHT_PID_CONTROLLER_H
