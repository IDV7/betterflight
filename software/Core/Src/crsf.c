#include "crsf.h"
#include "main.h"
#include "log.h"
#include "misc.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal.h"
#include "string.h"



uint8_t rx_data;
uint32_t * const modified_data;
static void crsf_uart_setup(crsf_handle_t *crsf_h);
//static uint8_t crc8(uint8_t *data, size_t len);
//static void crsf_parse_frame(crsf_handle_t *crsf_h);
//static void crsf_decode_payload(crsf_handle_t *crsf_h);
//static void crsf_error_handling(crsf_handle_t *crsf_h;
//static void crsf_convert_data(crsf_handle_t *crsf_h);
static void unpack_channels(uint8_t const * const payload, uint32_t * const dest);
static void crsf_telemetry_send(crsf_handle_t *crsf_h, crsf_frametype_t frame_type, uint8_t *data);

uint8_t crc8(uint8_t *data, size_t len) {
    uint8_t crc = 0;
    size_t i, j;

    for (i = 0; i < len; ++i) {
        crc ^= data[i];
        for (j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0xD5; // polynomial 0xD5
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

void crsf_telemetry_send(crsf_handle_t *crsf_h, crsf_frametype_t frame_type, uint8_t *data){
    //frame -> [sync/address] [len] [type] [payload] [crc8]
    uint8_t crc;
    uint8_t len = LEN_CORR(sizeof(data));

    uint8_t frame[len+2]; //add 2 for sync and address

    frame[0] = CRSF_ADDRESS_CRSF_RECEIVER; //address of receiver
    frame[1] = len; //length of frame
    if (len > 4){
        for(int i = 0; i < len-4; i++){
            frame[i+2] = data[i];
        }
        //calculate crc
        crc = crc8(data, len);

        for(int i = len-4; i < len; i++){
            frame[i] = data[i];
        }
        HAL_UART_Transmit(crsf_h->huart, frame, len+2, 1000);
        LOGD("Telemetry sent: %s", frame);
    }
    else{
        LOGD("Payload too small");
    }





}

void crsf_parse_frame(crsf_handle_t * crsf_h){

}
void crsf_decode_payload(crsf_handle_t * crsf_h){

}
void crsf_error_handling(crsf_handle_t * crsf_h){

}
void crsf_convert_data(crsf_handle_t * crsf_h){

}

void crsf_init(crsf_handle_t * crsf_h, UART_HandleTypeDef *huart) {
    crsf_h->huart = huart;
    crsf_uart_setup(crsf_h);
}
static void unpack_channels(uint8_t const * const payload, uint32_t * const dest)
{

    const unsigned dstBits = 32;
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
            readValue |= ((uint32_t) readByte) << bitsMerged;
            bitsMerged += 8;
        }
        //printf("rv=%x(%x) bm=%u\n", readValue, (readValue & inputChannelMask), bitsMerged);
        dest[n] = (readValue & inputChannelMask);
        readValue >>= CHANNEL_SIZE;
        bitsMerged -= CHANNEL_SIZE;
    }
}
void crsf_process(crsf_handle_t * crsf_h){
    // check if action has to be taken
    crsf_parse_frame(crsf_h);

    crsf_decode_payload(crsf_h);

    //process data
    crsf_error_handling(crsf_h);
    crsf_convert_data(crsf_h);

}

static void crsf_uart_setup(crsf_handle_t * crsf_h){
    crsf_h->huart->Instance = USART2;
    crsf_h->huart->Init.BaudRate = 420000;
    crsf_h->huart->Init.WordLength = UART_WORDLENGTH_8B;
    crsf_h->huart->Init.StopBits = UART_STOPBITS_1;
    crsf_h->huart->Init.Parity = UART_PARITY_NONE;
    crsf_h->huart->Init.Mode = UART_MODE_RX;
    crsf_h->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    crsf_h->huart->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init((UART_HandleTypeDef *) crsf_h->huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{


    if (huart->Instance == USART2)
    {

            // Data received, handle it
            // Example: Print received data

        HAL_UART_Receive_IT(&huart2, &rx_data, 26);
        unpack_channels(&rx_data, modified_data);
            // Start receiving again
        HAL_Delay(10);
        for (unsigned ch=0; ch<16; ++ch){
            LOGD("ch%02u=%u ", ch, modified_data[ch]);
            HAL_Delay(10);
        }
    }
}

void crsf_send_frame_test(UART_HandleTypeDef *huart){
    crsf_handle_t crsf_h;
    crsf_h.huart = huart;
    uint8_t tx_data;
    tx_data = 123;

    if (huart->Instance == USART2)
    {
        crsf_telemetry_send(&crsf_h, CRSF_FRAMETYPE_BATTERY_SENSOR, &tx_data);
        // Data received, handle it
        // Example: Print received data


        // Start receiving again
        //HAL_UART_Receive_IT(&huart2, &rx_data, 1);

    }
}

void crsf_unpack_channels_test(){
    uint32_t dst[16];
    uint8_t src[] = {
            0xc8, 0x18, 0x16,
            0xe0, 0x03, 0x1f, 0xf8, 0xc0, 0x07, 0x3e, 0xf0, 0x81, 0x0f, 0x7c,
            0xe0, 0x03, 0x1f, 0xf8, 0xc0, 0x07, 0x3e, 0xf0, 0x81, 0x0f, 0x7c, 0xad
    };
    unpack_channels(&src[3], dst);
    HAL_Delay(10);
    for (unsigned ch=0; ch<16; ++ch){
        LOGD("ch%02u=%u ", ch, dst[ch]);
        HAL_Delay(10);
    }

}

void crsf_tests(void){

    HAL_UART_RxCpltCallback(&huart2);
    //crsf_send_frame_test(&huart2);
    //crsf_unpack_channels_test();
}