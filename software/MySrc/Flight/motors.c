/*

 this makes use of the dshot driver to give a higher level of 4 motor control
 This does not include any PID control, only a better way to control 4 motors at a time.

  FRONT - TOP VIEW (QUAD COPTER MOTOR LAYOUT)
    |4|-||-|2|
        ||
    |3|-||-|1|

    IMPORTANT PROGRAMMING NOTE:
      ARRAY_LAYOUT: Only when using the array is the first motor "0" and the last motor "3"
      MOTOR_LAYOUT: When refering to motors anywhere else or not in the context of the arry, the first motor is "1" and the last motor is "4"

 */

#include "motors.h"
#include "dshot.h"
#include "log.h"
#include "misc.h"


static bool motors_are_off(motors_handle_t *motors_h);

void motors_init(motors_handle_t *motors_h, dshot_handle_t *m1_h, dshot_handle_t *m2_h, dshot_handle_t *m3_h, dshot_handle_t *m4_h) {
    motors_h->dshot_hs[0] = m1_h;
    motors_h->dshot_hs[1] = m2_h;
    motors_h->dshot_hs[2] = m3_h;
    motors_h->dshot_hs[3] = m4_h;

    for (int i = 0; i < 4; i++) {
        dshot_stop(motors_h->dshot_hs[i]);
    }
}

void motors_process(motors_handle_t *motors_h) {
    for (int i = 0; i < 4; i++) {
        dshot_process(motors_h->dshot_hs[i]);
    }
}

void motor_set_throttle(motors_handle_t *motors_h, uint8_t motor_num, uint16_t throttle) {
    dshot_set_throttle(motors_h->dshot_hs[motor_num - 1], throttle);
}

void motors_set_throttle_arr(motors_handle_t *motors_h, uint16_t throttle[4]) {
    for (int i = 0; i < 4; i++) {
        motor_set_throttle(motors_h, i + 1, throttle[i]);
    }
}

void motors_set_throttle(motors_handle_t *motors_h, uint16_t throttle_m1, uint16_t throttle_m2, uint16_t throttle_m3, uint16_t throttle_m4) {
    motor_set_throttle(motors_h, 1, throttle_m1);
    motor_set_throttle(motors_h, 2, throttle_m2);
    motor_set_throttle(motors_h, 3, throttle_m3);
    motor_set_throttle(motors_h, 4, throttle_m4);
}

void motors_stop(motors_handle_t *motors_h) {
    for (int i = 0; i < 4; i++) {
        motor_stop(motors_h, i+1);
    }
}

void motor_stop(motors_handle_t *motors_h, uint8_t motor_num) {
    dshot_set_throttle(motors_h->dshot_hs[motor_num - 1], 0);
}

void motors_beep(motors_handle_t *motors_h) {
    for (int i = 0; i < 4; i++) {
        dshot_send_special_command(motors_h->dshot_hs[i], DSHOT_CMD_BEEP1);
    }
}

void motor_beep(motors_handle_t *motors_h, uint8_t motor_num) {
    dshot_send_special_command(motors_h->dshot_hs[motor_num - 1], DSHOT_CMD_BEEP1);
}

// takes a motor number and sets it to one of the outputs (MOTOR_LAYOUT) (swaps 2 motors from place...)
void motor_set_layout(motors_handle_t *motors_h, dshot_handle_t *m_h, uint8_t motor_num) {
    //only allow this while motors are off
    if (!motors_are_off(motors_h)) {
        LOGE((uint8_t*)"Motors are not off, cannot change motor layout!");
        return;
    }

    //save the motor that is being overwritten
    dshot_handle_t *overwrite_backup = motors_h->dshot_hs[motor_num - 1];

    motors_h->dshot_hs[motor_num - 1] = m_h;

    for (int i = 0; i < 4; i++) {
        if (motors_h->dshot_hs[i]->htim->Instance == m_h->htim->Instance) {
            motors_h->dshot_hs[i] = overwrite_backup;
            break;
        }
    }
}

void motor_set_direction(motors_handle_t *motors_h, uint8_t motor_num, bool reverse) {
    if (reverse) {
        dshot_send_special_command(motors_h->dshot_hs[motor_num - 1], DSHOT_CMD_SPIN_DIRECTION_REVERSED);
        return;
    }
    dshot_send_special_command(motors_h->dshot_hs[motor_num - 1], DSHOT_CMD_SPIN_DIRECTION_NORMAL);
}

void motors_set_direction(motors_handle_t *motors_h, bool reverse_m1, bool reverse_m2, bool reverse_m3, bool reverse_m4) {
    motor_set_direction(motors_h, 1, reverse_m1);
    motor_set_direction(motors_h, 2, reverse_m2);
    motor_set_direction(motors_h, 3, reverse_m3);
    motor_set_direction(motors_h, 4, reverse_m4);
}

// ------- STATIC FUNCTIONS ------- //

static bool motors_are_off(motors_handle_t *motors_h) {
    for (int i = 0; i < 4; i++) {
        if (motors_h->dshot_hs[i]->throttle_value != 0) {
            return false;
        }
    }
    return true;
}




