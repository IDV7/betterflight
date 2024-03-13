//
// Created by Maarten on 29-2-2024.
//

#ifndef BETTERFLIGHT_PID_CONTROLLER_H
#define BETTERFLIGHT_PID_CONTROLLER_H
typedef struct {
float yaw_set_point;
float roll_set_point;
float pitch_set_point;
}set_points_t;

typedef struct{
    float yaw_stick_output;
    float roll_stick_output;
    float pitch_stick_output;
}stick_output_t;

typedef struct{
    set_points_t set_points;
    stick_output_t stick_outputs;
}pid_set_point_t;
void get_set_points(pid_set_point_t *set_points, float yaw_stick_output, float roll_stick_output,
                    float pitch_stick_output, float *yaw_set_point, float *roll_set_point, float *pitch_set_point);

#endif //BETTERFLIGHT_PID_CONTROLLER_H
