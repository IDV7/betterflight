#ifndef BETTERFLIGHT_MYMAIN_H
#define BETTERFLIGHT_MYMAIN_H

#include "misc.h"
#include "dshot.h"
#include "cli.h"
#include "motors.h"

// for precise timing related debugging (measured with logic analyzer)
#define S2_HIGH (GPIOB->BSRR = GPIO_PIN_4)
#define S2_LOW (GPIOB->BSRR = (GPIO_PIN_4 << 16))


extern cli_handle_t cli_h;



//motors
//extern dshot_handle_t *m1_h; // TIM1 CH2
//extern dshot_handle_t *m2_h; // TIM1 CH1
//extern dshot_handle_t *m3_h; // TIM8 CH4
//extern dshot_handle_t *m4_h; // TIM8 CH3
//extern dshot_handle_t *m_hs[4];
extern motors_handle_t motors_h;

void myinit(void);
void mymain(void);

#endif //BETTERFLIGHT_MYMAIN_H
