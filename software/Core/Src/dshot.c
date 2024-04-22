/*
 This is a dshot driver for stm32(f7) microcontrollers that makes use of the stm32 HAL library.
 It is almost 100% portable, you will just need to remove my project specific logging functions and includes.
 You will also have to reconfigure the PSC and ARR values to match your peripheral clock speed.
 */


#include "dshot.h"
#include "main.h"
#include "log.h"
#include "stm32f7xx_hal_tim.h"


static void dshot_prepare_packet(uint32_t *dma_buffer ,uint16_t value);
static void dshot_dma_complete_callback(DMA_HandleTypeDef *hdma);
static uint32_t dshot_find_ccrx(dshot_handle_t *dshot_h);
static void dshot_dma_enable(DMA_HandleTypeDef *hdma, bool enable);
static void dshot_send(dshot_handle_t *dshot_h, uint16_t* motor_value);
static dshot_cmd_info_t* dshot_find_special_cmd_info(dshot_special_cmd_t cmd);

void dshot_init(dshot_handle_t *dshot_h, TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma, uint32_t tim_channel) {
    dshot_h->htim = htim;
    dshot_h->hdma = hdma;
    dshot_h->tim_channel = tim_channel;
    dshot_h->throttle_value = 0;

    // set timer
    __HAL_TIM_SET_PRESCALER(dshot_h->htim, DSHOT_PSC);
    __HAL_TIM_SET_AUTORELOAD(dshot_h->htim, DSHOT_ARR);
    HAL_TIM_PWM_Start(dshot_h->htim, dshot_h->tim_channel);

    //set dma callback function
    dshot_h->hdma->XferCpltCallback = dshot_dma_complete_callback;
}

// CALL EVERY 1MS or esc will disconnect
void dshot_process(dshot_handle_t * dshot_h) {

    // warm user if dshot_process is not called every 2ms, esc might disarm if not send a new frame every so often
    static uint64_t last_time = 0;
    uint64_t current_time = millis;
    if (last_time != 0 && (current_time - last_time) > 2) {
        LOGW((uint8_t*)"More than 1ms has been past since DSHOT PROCESS has been called, motor may disconnect!");
    }
    last_time = current_time;

    //check if we have to send a special command
    if (dshot_h->send_special_cmd_count > 0) {
        dshot_send(dshot_h, (uint16_t*)&dshot_h->special_cmd);
        dshot_h->send_special_cmd_count--;
        return;
    }

    //send stored value to motor
    dshot_send(dshot_h, &dshot_h->throttle_value);
}

void dshot_stop(dshot_handle_t *dshot_h) {
    dshot_h->throttle_value = 0;
}

//doesn't change the speed, expects you to call dshot_process every 1ms
void dshot_set_speed(dshot_handle_t *dshot_h, uint16_t throttle) {
    if (throttle < DSHOT_MIN_THROTTLE) {
        throttle = DSHOT_MIN_THROTTLE;
    } else if (throttle > DSHOT_MAX_THROTTLE) {
        throttle = DSHOT_MAX_THROTTLE;
    }

    dshot_h->throttle_value = throttle - DSHOT_MIN_THROTTLE;
}

void dshot_send_special_command(dshot_handle_t *dshot_h, dshot_special_cmd_t cmd) {
    dshot_h->special_cmd = cmd;
    dshot_cmd_info_t *cmd_info = dshot_find_special_cmd_info(cmd);

    //refuse cmd if it requires motor to be stopped and the motor is still spinning
    if (cmd_info->cmd (stopped here for the day) CMD_ONLY_WHILE_STOP_RANGE && dshot_h->throttle_value > 0) {
        LOGW((uint8_t*)"Special command %d refused, motor is still spinning", cmd);
        return;
    }


    dshot_h->send_special_cmd_count = cmd_info->send_count;



}

// ---------- static functions ---------- //

// raw send function, use dshot_set_speed to set motor speed, dshot_send_special_command to send special commands
static void dshot_send(dshot_handle_t *dshot_h, uint16_t *value) {
    dshot_prepare_packet(dshot_h->dma_buffer, *value);

    HAL_DMA_Start_IT(dshot_h->hdma, (uint32_t) dshot_h->dma_buffer, dshot_find_ccrx(dshot_h), DSHOT_DMA_BUFFER_SIZE);
    dshot_dma_enable(dshot_h->hdma, true);
    // when dma is done, the dma callback function will disable the dma channel.
}

static void dshot_prepare_packet(uint32_t *dma_buffer ,uint16_t value) {
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

    // the dma buffer represents pwm duty cycles for each bit in the frame (the values depends on PSC and ARR)
    // put packet in dma buffer with correct pwm duty cycle
    for (int i = 0; i < DSHOT_FRAME_SIZE; i++) {
        dma_buffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
        packet <<= 1;
    }
    dma_buffer[16] = 0;
    dma_buffer[17] = 0;
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

static uint32_t dshot_find_ccrx(dshot_handle_t *dshot_h) {
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

static dshot_cmd_info_t* dshot_find_special_cmd_info(dshot_special_cmd_t cmd) {
    for (int i = 0; i < DSHOT_CMD_SEND_INFO_SIZE; i++) {
        if (DSHOT_CMD_SEND_INFO[i].cmd == cmd) {
            return &DSHOT_CMD_SEND_INFO[i];
        }
    }

    // return standard info if not found, this is normal, only cmd's with special requirements are in the table
    dshot_cmd_info_t standard_cmd_info = {cmd, 1, 0};
    return &standard_cmd_info;
}


