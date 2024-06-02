/*
 * set_pids function is used calculate all the 3 pids values using 1 function.
 * For the specifics of each pid, the pid_controller_update (pid.h) function is used.
 */

#include "set_point.h"
#include "pid.h"
#include "mixer.h"


#ifndef BETTERFLIGHT_PID_CONTROLLER_H
#define BETTERFLIGHT_PID_CONTROLLER_H


typedef struct {
    pid_handle_t yaw_pid;
    pid_handle_t roll_pid;
    pid_handle_t pitch_pid;
}pids_t;

typedef struct{
    set_points_t set_p;
    flight_axis_t data;
    pids_val_t pid_val;
    set_point_handle_t setp;
    pids_t pid;
}flight_pids_t;


void set_pids(flight_pids_t *drone_pids, flight_axis_int16_t *flight_measurements, set_points_t *set_ps, pids_val_t *pid_vals);
void pid_init(flight_pids_t *drone_pids);
void  pids_clear(flight_pids_t *drone_pids);

#endif //BETTERFLIGHT_PID_CONTROLLER_H
