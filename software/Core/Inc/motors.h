
#ifndef BETTERFLIGHT_MOTORS_H
#define BETTERFLIGHT_MOTORS_H

#include "dshot.h"

#define M1_H(motors_h) ((motors_h)->dshot_hs[0])
#define M2_H(motors_h) ((motors_h)->dshot_hs[1])
#define M3_H(motors_h) ((motors_h)->dshot_hs[2])
#define M4_H(motors_h) ((motors_h)->dshot_hs[3])

typedef struct {
    dshot_handle_t dshot_hs[4];
} motors_handle_t;

void motors_init(motors_handle_t *motors_h, dshot_handle_t m1_h, dshot_handle_t m2_h, dshot_handle_t m3_h, dshot_handle_t m4_h);

#endif //BETTERFLIGHT_MOTORS_H
