
#include "dshot.h"
#include "main.h"
#include <math.h>
#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal_tim.h"


static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_dma_complete_callback(DMA_HandleTypeDef *hdma);
uint32_t dshot_find_ccrx(dshot_handle_t *dshot_h);
static void dshot_dma_enable(DMA_HandleTypeDef *hdma, bool enable);

void dshot_init(dshot_handle_t *dshot_h, TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma, uint32_t tim_channel) {
    dshot_h->htim = htim;
    dshot_h->hdma = hdma;
    dshot_h->tim_channel = tim_channel;
    dshot_h->motor_value = 0;

    // set timer
//    uint16_t dshot_prescaler = (lrintf((float)TIMER_CLOCK/ (float)DSHOT600_HZ + 0.01f) - 1);
    __HAL_TIM_SET_PRESCALER(dshot_h->htim, DSHOT_PSC);
    __HAL_TIM_SET_AUTORELOAD(dshot_h->htim, DSHOT_ARR);
    HAL_TIM_PWM_Start(dshot_h->htim, dshot_h->tim_channel);


    //set dma callback function
    dshot_h->hdma->XferCpltCallback = dshot_dma_complete_callback;
}

// CALL EVERY 1MS or esc will disconnect
void dshot_process(dshot_handle_t * dshot_h) {
    static uint64_t last_time = 0;
    uint64_t current_time = millis;
    if (last_time != 0 && (current_time - last_time) > 2) {
        LOGW((uint8_t*)"More than 1ms has been past since DSHOT PROCESS has been called, motor may disconnect!");
    }
    last_time = current_time;

    //command dma to send new value to motor
    dshot_send(dshot_h, &dshot_h->motor_value);
}

void dshot_stop(dshot_handle_t *dshot_h) {
    dshot_h->motor_value = 0;
}

//doesn't change the speed, expects you to call dshot_process every 1ms
void dshot_set_speed(dshot_handle_t *dshot_h, uint16_t throttle) {
    if (throttle < DSHOT_MIN_THROTTLE) {
        throttle = DSHOT_MIN_THROTTLE;
    } else if (throttle > DSHOT_MAX_THROTTLE) {
        throttle = DSHOT_MAX_THROTTLE;
    }

    dshot_h->motor_value = throttle - DSHOT_MIN_THROTTLE;
}




void dshot_send(dshot_handle_t *dshot_h, uint16_t *value) {

    // put packet in dma buffer
    uint16_t packet = dshot_prepare_packet(*value);
    for (int i = 0; i < DSHOT_FRAME_SIZE; i++) {
        dshot_h->dma_buffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
        packet <<= 1;
    }
    dshot_h->dma_buffer[16] = 0;
    dshot_h->dma_buffer[17] = 0;

    HAL_DMA_Start_IT(dshot_h->hdma, (uint32_t) dshot_h->dma_buffer, dshot_find_ccrx(dshot_h), DSHOT_DMA_BUFFER_SIZE);
    dshot_dma_enable(dshot_h->hdma, true);
    // when dma is done, the dma callback function will disable the dma channel.
}

static uint16_t dshot_prepare_packet(uint16_t value) {
    uint16_t packet = 0;
    bool dshot_telemetry = false;

    packet = (value << 1) | (dshot_telemetry ? 1 : 0);

    //calculate checksum
    unsigned csum = 0;
    unsigned csum_data = packet;

    for (int i = 0; i < 3; i++) {
        csum ^= csum_data;
        csum_data >>= 4;
    }

    csum &= 0xf;
    packet = (packet << 4) | csum;

    return packet;
}


static void dshot_dma_complete_callback(DMA_HandleTypeDef *hdma) {
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
    // find the correct dma channel and disable it
    dshot_dma_enable(hdma, false);
}

// true for enable, false for disable
static void dshot_dma_enable(DMA_HandleTypeDef *hdma, bool enable) {
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

    if (enable) {
        if (hdma == htim->hdma[TIM_DMA_ID_CC1])
            __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC2])
            __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC3])
            __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC4])
            __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
        else
            LOGE("DMA channel not found");
    } else {
        if (hdma == htim->hdma[TIM_DMA_ID_CC1])
            __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC2])
            __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC3])
            __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
        else if(hdma == htim->hdma[TIM_DMA_ID_CC4])
            __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
        else
            LOGE("DMA channel not found");

    }
}


uint32_t dshot_find_ccrx(dshot_handle_t *dshot_h) {
    if (dshot_h->tim_channel == TIM_CHANNEL_1) {
        return (uint32_t)&dshot_h->htim->Instance->CCR1;
    } else if (dshot_h->tim_channel == TIM_CHANNEL_2) {
        return (uint32_t)&dshot_h->htim->Instance->CCR2;
    } else if (dshot_h->tim_channel == TIM_CHANNEL_3) {
        return (uint32_t)&dshot_h->htim->Instance->CCR3;
    } else if (dshot_h->tim_channel == TIM_CHANNEL_4) {
        return (uint32_t)&dshot_h->htim->Instance->CCR4;
    } else {
        LOGE("Invalid TIM channel");
        return 0;
    }
}

