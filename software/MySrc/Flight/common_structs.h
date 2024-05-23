#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

#include <stdio.h>

//needed for mixer and motor control
typedef struct {
    int16_t motor1;
    int16_t motor2;
    int16_t motor3;
    int16_t motor4;
}motor_output_t;

//needed for CRSF and set_point
typedef struct{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    int16_t thr;
}flight_axis_t;

typedef struct
{
    int16_t sp;
}set_point_t;

typedef struct {
    set_point_t yaw_set_point;
    set_point_t roll_set_point;
    set_point_t pitch_set_point;

}set_points_t;

typedef struct{
    int16_t yaw_pid;
    int16_t roll_pid;
    int16_t pitch_pid;
    int16_t thr_pid;
}pids_val_t;


typedef struct{
    int16_t min_stick_output;
    int16_t max_stick_output;
}stick_limits_t;


typedef struct
{
    int16_t yaw;
    int16_t roll;
    int16_t pitch;

}flight_measurements_t;






#endif //COMMON_STRUCTS_H
