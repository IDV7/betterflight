
#ifndef BETTERFLIGHT_MOTORS_H
#define BETTERFLIGHT_MOTORS_H

#include "dshot.h"

typedef struct {
    dshot_handle_t *dshot_hs[4];
} motors_handle_t;

// initializes handle
void motors_init(motors_handle_t *motors_h, dshot_handle_t *m1_h, dshot_handle_t *m2_h, dshot_handle_t *m3_h, dshot_handle_t *m4_h);

// processes all motors (all dshot handles)
void motors_process(motors_handle_t *motors_h);

// sets throttle for all motors
void motors_set_throttle(motors_handle_t *motors_h, uint16_t throttle_m1, uint16_t throttle_m2, uint16_t throttle_m3, uint16_t throttle_m4);

// sets throttle for all motors using an array of throttle values
void motors_set_throttle_arr(motors_handle_t *motors_h, uint16_t throttle[4]);

// sets throttle for a single motor
void motor_set_throttle(motors_handle_t *motors_h, uint8_t motor_num, uint16_t throttle);

// stops all motors
void motors_stop(motors_handle_t *motors_h);

// stops a single motor
void motor_stop(motors_handle_t *motors_h, uint8_t motor_num);

// sets layout for motors
void motor_set_layout(motors_handle_t *motors_h, dshot_handle_t *m_h, uint8_t motor_num);

// sets direction for a single motor
void motor_set_direction(motors_handle_t *motors_h, uint8_t motor_num, bool reverse);

// sets direction for all motors
void motors_set_direction(motors_handle_t *motors_h, bool reverse_m1, bool reverse_m2, bool reverse_m3, bool reverse_m4);

// beeps all motors
void motors_beep(motors_handle_t *motors_h);

#endif //BETTERFLIGHT_MOTORS_H
