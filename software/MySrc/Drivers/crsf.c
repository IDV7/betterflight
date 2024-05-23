#include "crsf.h"
#include "main.h"
#include "log.h"
#include "misc.h"
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
bool print_start_flag = false;
uint8_t crc8;
uint8_t crc8_calculate(uint8_t *data, uint8_t len);
static void crsf_uart_setup(crsf_handle_t *crsf_h);
static void unpack_channels(uint8_t const * payload, uint32_t * dest);
static void crsf_telemetry_send(crsf_handle_t *crsf_h, crsf_frametype_t frame_type, uint8_t *data);



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
        crc = crc8_calculate(data, len);

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


void crsf_init(crsf_handle_t * crsf_h, UART_HandleTypeDef *huart) {
    crsf_h->huart = huart;
    crsf_uart_setup(crsf_h);
}

static void unpack_channels(uint8_t const * payload, uint32_t * dest)
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

void crsf_process(crsf_handle_t * crsf_h, flight_axis_t * data){
    if(state == processing_data){

        if(start_data_saved[2] == CRSF_FRAMETYPE_RC_CHANNELS_PACKED){
            LOGD("Processing data");
            HAL_Delay(10);
            uint32_t channels[16];
            crc8 = incoming_data_saved[frame_length-2];
            uint8_t incoming_frame_lenght = frame_length;
            LOGD("Frame length: %u", incoming_frame_lenght);
            HAL_Delay(10);
            unpack_channels(incoming_data_saved, channels);
            data->yaw = channels[0];
            data->pitch = channels[1];
            data->thr = channels[2];
            data->roll = channels[3];



            for (unsigned ch=0; ch<16; ++ch){
                LOGD("ch%02u=%u", ch+1, channels[ch]);

                HAL_Delay(10);
            }
            LOGD("CRC8=%u", crc8);
            state = wait_for_sync;
            HAL_UART_Receive_IT(crsf_h->huart, start_data, 3);
        }


    }
    else if(state == wait_for_sync){
        HAL_UART_Receive_IT(crsf_h->huart, start_data, 3);
        LOGD("Waiting for sync");
        HAL_Delay(10);
    }
    else if(state == receiving_data){


        HAL_UART_Receive_IT(crsf_h->huart, incoming_data, start_data_saved[1]);
        //LOGD("Receiving data");
        if (print_start_flag == true){
            for (int i = 0; i < 3; ++i) {

                LOGD("start_data[%u]=%u ", i, start_data_saved[i]);
                HAL_Delay(10);
            }
            print_start_flag = false;
        }

    }
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (huart->Instance == USART2) {
    if(state == wait_for_sync){
        //HAL_UART_Receive_IT(huart, start_data, 3);
        if(start_data[0] == 0xC8){
            print_start_flag = true;
            if(start_data[1] > 0 && start_data[1] < CRSF_MAX_PACKET_SIZE){
                start_data_saved[0] = start_data[0];
                start_data_saved[1] = start_data[1];
                start_data_saved[2] = start_data[2];
                frame_length = start_data[1];

                HAL_UART_Receive_IT(huart, incoming_data, frame_length-1);
                state = receiving_data;
            }
            else{
                state = receiving_data;
                HAL_UART_Receive_IT(huart, start_data, 3);
            }

        }
        else{
            state = wait_for_sync;
            HAL_UART_Receive_IT(huart, start_data, 3);
        }
    }
    else if(state == receiving_data){
        //do something
        HAL_UART_Receive_IT(huart, incoming_data, frame_length-1);
        for (int i = 0; i < frame_length; ++i) {
            incoming_data_saved[i] = incoming_data[i];
        }
        state = processing_data;
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


    //crsf_send_frame_test(&huart2);
    //crsf_unpack_channels_test();
}