
#include "pid_controller.h"



#define T 0.000625
#define KProll 2
#define KIroll 0.1
#define KDroll 0.5

#define KPpitch KProll
#define KIpitch KIroll
#define KDpitch KDroll

#define KPyaw 1
#define KIyaw 0.1
#define KDyaw 0.2



void pids_clear(flight_pids_t *drone_pids){

    pid_controller_clear(&drone_pids->pid.yaw_pid);
    pid_controller_clear(&drone_pids->pid.roll_pid);
    pid_controller_clear(&drone_pids->pid.pitch_pid);

}

void pid_init(flight_pids_t *drone_pids){

    pid_controller_init(&drone_pids->pid.yaw_pid, KPyaw, KIyaw, KDyaw, T, -500, 500, 1000, 2000);
    pid_controller_init(&drone_pids->pid.roll_pid, KProll, KIroll, KDroll, T, -500, 500, 1000, 2000);
    pid_controller_init(&drone_pids->pid.pitch_pid, KPpitch, KIpitch, KDpitch, T, -500, 500, 1000, 2000);

    limits_init(&drone_pids->setp, drone_pids->pid.yaw_pid.limits.min_output, drone_pids->pid.yaw_pid.limits.max_output);
    limits_init(&drone_pids->setp, drone_pids->pid.roll_pid.limits.min_output, drone_pids->pid.roll_pid.limits.max_output);
    limits_init(&drone_pids->setp, drone_pids->pid.pitch_pid.limits.min_output, drone_pids->pid.pitch_pid.limits.max_output);

}

void set_pids(flight_pids_t *drone_pids, flight_axis_int16_t *flight_measurements, set_points_t *set_ps, pids_val_t *pid_vals){

    drone_pids->pid_val.yaw_pid = pid_controller_update(&drone_pids->pid.yaw_pid, &pid_vals->yaw_pid, set_ps->yaw_set_point.sp, flight_measurements->yaw);
    drone_pids->pid_val.roll_pid = pid_controller_update(&drone_pids->pid.roll_pid, &pid_vals->roll_pid, set_ps->roll_set_point.sp, flight_measurements->roll);
    drone_pids->pid_val.pitch_pid = pid_controller_update(&drone_pids->pid.pitch_pid, &pid_vals->pitch_pid, set_ps->pitch_set_point.sp, flight_measurements->pitch);

}
