
#ifndef BETTERFLIGHT_MOTORS_H
#define BETTERFLIGHT_MOTORS_H

#include "dshot.h"

typedef struct {
    dshot_handle_t *dshot_hs[4];
} motors_handle_t;

void motors_init(motors_handle_t *motors_h, dshot_handle_t *m1_h, dshot_handle_t *m2_h, dshot_handle_t *m3_h, dshot_handle_t *m4_h);
void motors_process(motors_handle_t *motors_h);
void motors_set_throttle(motors_handle_t *motors_h, uint16_t throttle_m1, uint16_t throttle_m2, uint16_t throttle_m3, uint16_t throttle_m4);
void motors_set_throttle_arr(motors_handle_t *motors_h, uint16_t throttle[4]);
void motor_set_throttle(motors_handle_t *motors_h, uint8_t motor_num, uint16_t throttle);
void motors_stop(motors_handle_t *motors_h);
void motor_stop(motors_handle_t *motors_h, uint8_t motor_num);
void motor_set_layout(motors_handle_t *motors_h, dshot_handle_t *m_h, uint8_t motor_num);
void motor_set_direction(motors_handle_t *motors_h, uint8_t motor_num, bool reverse);
void motors_set_direction(motors_handle_t *motors_h, bool reverse_m1, bool reverse_m2, bool reverse_m3, bool reverse_m4);
void motors_beep(motors_handle_t *motors_h);

#endif //BETTERFLIGHT_MOTORS_H
