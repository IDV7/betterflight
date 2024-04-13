
#include "dshot.h"
#include "main.h"
#include <math.h>
#include "log.h"
#include "misc.h"


static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_dma_complete_callback(DMA_HandleTypeDef *hdma);

void dshot_init(dshot_handle_t *dshot, TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma, uint32_t tim_channel) {
    dshot->htim = htim;
    dshot->hdma = hdma;
    dshot->tim_channel = tim_channel;

    // set timer
    __HAL_TIM_SET_PRESCALER(dshot->htim, lrintf((float)TIMER_CLOCK / (float)DSHOT600_HZ + 0.01f) - 1);
    __HAL_TIM_SET_AUTORELOAD(dshot->htim, MOTOR_BITLENGTH);
    HAL_TIM_PWM_Start(dshot->htim, dshot->tim_channel);

    //set dma callback function
    dshot->hdma->XferCpltCallback = dshot_dma_complete_callback;
}

void dshot_send_throttle(dshot_handle_t *dshot, uint16_t throttle) {
    if (throttle < DSHOT_MIN_THROTTLE) {
        throttle = DSHOT_MIN_THROTTLE;
    } else if (throttle > DSHOT_MAX_THROTTLE) {
        throttle = DSHOT_MAX_THROTTLE;
    }

    uint16_t motor_value = throttle - DSHOT_MIN_THROTTLE;
    dshot_send(dshot, &motor_value);
}

void dshot_send(dshot_handle_t *dshot, uint16_t *value) {

    // put packet in dma buffer
    uint16_t packet = dshot_prepare_packet(*value);
    for (int i = 0; i < DSHOT_FRAME_SIZE; i++) {
        dshot->dma_buffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
        packet <<= 1;
    }
    dshot->dma_buffer[16] = 0;
    dshot->dma_buffer[17] = 0;

    for (int i = 0; i < DSHOT_FRAME_SIZE + 2; i++) {
        LOGD("dma_buffer[%d]: %d", i, dshot->dma_buffer[i]);
        delay(2);
    }

    HAL_DMA_Start_IT(dshot->hdma, (uint32_t) dshot->dma_buffer, (uint32_t)&dshot->htim->Instance->CCR1, DSHOT_DMA_BUFFER_SIZE);
    __HAL_TIM_ENABLE_DMA(dshot->htim, TIM_DMA_CC1);
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
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    // find the correct dma channel and disable it
    /*if (hdma == htim->hdma[TIM_DMA_ID_CC1])
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    else if(hdma == htim->hdma[TIM_DMA_ID_CC2])
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
    else if(hdma == htim->hdma[TIM_DMA_ID_CC3])
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
    else if(hdma == htim->hdma[TIM_DMA_ID_CC4])
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
    else
        LOGE("DMA channel not found");*/
}