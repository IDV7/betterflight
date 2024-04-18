/*

 This is a DSHOT driver for stm32(f7) microcontrollers.
 Inspiration from:
    - betaflight: https://betaflight.com/ocs/development/dshot/
    - mokhwasomssi: https://github.com/mokhwasomssi/stm32_hal_dshot

*/

#ifndef BETTERFLIGHT_DSHOT_H
#define BETTERFLIGHT_DSHOT_H

#include "main.h"
#include "misc.h"


#define TIMER_CLOCK 100000000 // 100MHz

#define MOTOR_BIT_0 240
#define MOTOR_BIT_1 480
#define DSHOT_PSC 0
#define DSHOT_ARR 639
#define MOTOR_BITLENGTH 20

#define DSHOT_FRAME_SIZE 16
#define DSHOT_DMA_BUFFER_SIZE 18 // 16 data bits + 2 stop bits (reset)

#define DSHOT_MIN_THROTTLE 48
#define DSHOT_MAX_THROTTLE 2047
#define DSHOT_RANGE (DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

#define MHZ_TO_HZ(x) 			((x) * 1000000)

#define DSHOT600_HZ     		MHZ_TO_HZ(12)
#define DSHOT300_HZ     		MHZ_TO_HZ(6)
#define DSHOT150_HZ     		MHZ_TO_HZ(3)

typedef enum {
    DSHOT150 = 150,
    DSHOT300 = 300,
    DSHOT600 = 600
} dshot_type_t;

typedef struct {
    uint32_t dma_buffer[DSHOT_DMA_BUFFER_SIZE];
    TIM_HandleTypeDef *htim;
    DMA_HandleTypeDef *hdma;
    uint32_t tim_channel;
} dshot_handle_t;

void dshot_init(dshot_handle_t *dshot_h, TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma, uint32_t tim_channel);
void dshot_send(dshot_handle_t *dshot_h, uint16_t* motor_value);
void dshot_send_throttle(dshot_handle_t *dshot, uint16_t throttle);

#endif //BETTERFLIGHT_DSHOT_H
