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


void motors_init(motors_handle_t *motors_h, dshot_handle_t m1_h, dshot_handle_t m2_h, dshot_handle_t m3_h, dshot_handle_t m4_h) {
    motors_h->dshot_hs[0] = m1_h;
    motors_h->dshot_hs[1] = m2_h;
    motors_h->dshot_hs[2] = m3_h;
    motors_h->dshot_hs[3] = m4_h;
}

// takes a motor number and returns the dshot handle (MOTOR_LAYOUT)
void motor_set_layout(motors_handle_t *motors_h, dshot_handle_t m_h, uint8_t motor_num) {
    motors_h->dshot_hs[motor_num - 1] = m_h;
}

void motor_set_direction(motors_handle_t *motors_h, uint8_t motor_num, bool reverse) {
    dshot_set_direction(&motors_h->dshot_hs[motor_num - 1], reverse);
}




