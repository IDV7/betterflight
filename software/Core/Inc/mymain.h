#ifndef BETTERFLIGHT_MYMAIN_H
#define BETTERFLIGHT_MYMAIN_H

#include "misc.h"
#include "dshot.h"
#include "cli.h"
#include "gyro.h"

extern gyro_t gyro_h;
extern cli_handle_t cli_h;

//motors
extern dshot_handle_t hm1; // TIM1 CH2
extern dshot_handle_t hm2; // TIM1 CH1
extern dshot_handle_t hm3; // TIM8 CH4
extern dshot_handle_t hm4; // TIM8 CH3

void myinit(void);
void mymain(void);

#endif //BETTERFLIGHT_MYMAIN_H
