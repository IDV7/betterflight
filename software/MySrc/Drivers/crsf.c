#include "crsf.h"
#include "main.h"
#include "log.h"

#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal.h"
#include "string.h"
#include "common_structs.h"




crsf_receive_state_t state;

uint8_t start_data[3];
uint8_t start_data_saved[3];
uint8_t incoming_data[60];
uint8_t incoming_data_saved[60];
uint8_t frame_length;

uint8_t crc8;
uint8_t crc8_data_to_calc[25];
uint8_t crc8_calculate(uint8_t *data, uint8_t len);
static void crsf_uart_setup(crsf_handle_t *crsf_h);
static void unpack_channels(uint8_t const * payload, uint32_t * dest);


/*
 * gets called from main loop
 * 3 states posible
 * wait_for_sync: waiting to receive address, length and type
 * call HAL_UART_Receive_IT to receive the address, length and type
 * receiving_data: receiving the channel data
 * call HAL_UART_Receive_IT to receive the channel data
 * processing_data: all the data is received and can be processed
 * if processing is done the state is set to wait_for_sync and hal_uart_receive_it is called to receive the address, length and type


 */

void crsf_process(crsf_handle_t * crsf_h, uint32_t *data, bool *crc8_confirmed_flag){
    if(state == processing_data){

        if(start_data_saved[2] == CRSF_FRAMETYPE_RC_CHANNELS_PACKED){

            crc8_data_to_calc[0] = start_data_saved[2]; // we need frame time for crc8 calculation

            //add the channel data to the crc8_data_to_calc array for crc8 calculation
            for (int i = 0; i < 22; i++) {
                crc8_data_to_calc[i+1] = incoming_data_saved[i];
            }


            uint8_t crc8_calculated = crc8_calculate(crc8_data_to_calc, 23);
            crc8 = incoming_data_saved[22];



//            error needs to be added so that if crc8 is wrong for a long time
            if(crc8_calculated == crc8) {
                unpack_channels(incoming_data_saved, data);
                *crc8_confirmed_flag = true;
            }

            state = wait_for_sync;
            HAL_UART_Receive_IT(crsf_h->huart, start_data, 3);
        }
        else{

            state = wait_for_sync;
            HAL_UART_Receive_IT(crsf_h->huart, start_data, 3);
        }


    }
    else if(state == wait_for_sync){
        HAL_UART_Receive_IT(crsf_h->huart, start_data, 3);

    }
    else if(state == receiving_data){


        HAL_UART_Receive_IT(crsf_h->huart, incoming_data, start_data_saved[1]);



    }
}
uint8_t crc8_calculate(uint8_t *data, uint8_t len) {
    // Implement CRC-8 calculation based on the polynomial used in CRSF
    uint8_t crc = 0;

    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0xD5; // Polynomial used in CRSF
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}





static void unpack_channels(uint8_t const * payload, uint32_t * dest)
{


    const unsigned inputChannelMask = (1 << CHANNEL_SIZE) - 1;

    // code from BetaFlight rx/crsf.cpp / bitpacker_unpack
    uint8_t bitsMerged = 0;
    uint32_t readValue = 0;
    unsigned readByteIndex = 0;
    for (uint8_t n = 0; n < NUM_CHANNELS; n++)
    {
        while (bitsMerged < CHANNEL_SIZE)
        {
            uint8_t readByte = payload[readByteIndex++];
            readValue |= (readByte) << bitsMerged;
            bitsMerged += 8;
        }

        dest[n] = (int16_t)(readValue & inputChannelMask);

        readValue >>= CHANNEL_SIZE;
        bitsMerged -= CHANNEL_SIZE;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (huart->Instance == USART2) {
    if(state == wait_for_sync){
        //HAL_UART_Receive_IT(huart, start_data, 3);
        if(start_data[0] == 0xC8){

            if(start_data[1] > 0 && start_data[1] < CRSF_MAX_PACKET_SIZE){
                start_data_saved[0] = start_data[0];
                start_data_saved[1] = start_data[1];
                start_data_saved[2] = start_data[2];
                frame_length = start_data[1]-1;

                HAL_UART_Receive_IT(huart, incoming_data, frame_length);
                state = receiving_data;
            }
            else{
                state = wait_for_sync;
                HAL_UART_Receive_IT(huart, start_data, 3);
            }

        }
        else{
            state = wait_for_sync;
            HAL_UART_Receive_IT(huart, start_data, 3);
        }
    }
    else if(state == receiving_data){

       //HAL_UART_Receive_IT(huart, incoming_data, frame_length);

       //dont need interupts when copying data
        __disable_irq();
        for (int i = 0; i < frame_length; i++) {
            incoming_data_saved[i] = incoming_data[i];
        }
        __enable_irq();

        state = processing_data;
    }

    }


}

void crsf_init(crsf_handle_t * crsf_h, UART_HandleTypeDef *huart) {
    crsf_h->huart = huart;
    crsf_uart_setup(crsf_h);
}

static void crsf_uart_setup(crsf_handle_t * crsf_h) {

    LOGD("UART setup begin");
    HAL_Delay(5);
    crsf_h->huart->Instance = USART2;
    crsf_h->huart->Init.BaudRate = 420000;
    crsf_h->huart->Init.WordLength = UART_WORDLENGTH_8B;
    crsf_h->huart->Init.StopBits = UART_STOPBITS_1;
    crsf_h->huart->Init.Parity = UART_PARITY_NONE;
    crsf_h->huart->Init.Mode = UART_MODE_RX;
    crsf_h->huart->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init((UART_HandleTypeDef *) crsf_h->huart);
    LOGD("UART setup done");
    HAL_Delay(20);
    state = wait_for_sync;

}