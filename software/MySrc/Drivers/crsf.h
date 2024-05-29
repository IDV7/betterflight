

#include "stm32f7xx.h"
#include "stm32f7xx_hal_uart.h"
#include "common_structs.h"
/*
crsf notes:

 frame -> [sync/address] [len] [type] [payload] [crc8]

 [sync] : Begin with 0xC8 or specific address for telemetry
 [len]  length = length.payload + 4
 [type] (crsf_frametype_t enum)
 [payload] :
 [crc8] :

 */

#define CRSF_SYNC 0XC8
#define CRSF_ADDRESS_CRSF_RECEIVER 0xEC
#define LEN_CORR(x) x+4
#define NUM_CHANNELS 16
#define CHANNEL_SIZE 11
#define CRSF_MAX_PACKET_SIZE 64

//payload

typedef enum {
    CRSF_FRAME_GPS_PAYLOAD_SIZE = 15,
    CRSF_FRAME_BATTERY_SENSOR_PAYLOAD_SIZE = 8,
    CRSF_FRAME_LINK_STATISTICS_PAYLOAD_SIZE = 10,
    CRSF_FRAME_RC_CHANNELS_PAYLOAD_SIZE = 22, // 11 bits per channel * 16 channels = 22 bytes.
    CRSF_FRAME_ATTITUDE_PAYLOAD_SIZE = 6,
}crsf_payload_sizes_t;

typedef enum{
   wait_for_sync = 1,
   receiving_data = 2,
   processing_data = 3
}crsf_receive_state_t;

typedef enum { //https://github.com/crsf-wg/crsf/wiki/Packet-Types
    CRSF_FRAMETYPE_GPS = 0x02,
    CRSF_FRAMETYPE_VARIO  = 0x07,
    CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
    CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
    CRSF_FRAMETYPE_HEARTBEAT = 0x0B,       //imp
    CRSF_FRAMETYPE_LINK_STATISTICS =  0x14,
    CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
    CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED = 0x17,
    CRSF_FRAMETYPE_LINK_RX_ID = 0x1C,
    CRSF_FRAMETYPE_LINK_TX_ID = 0x1D,
    CRSF_FRAMETYPE_ATTITUDE = 0x1e,
    CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,
    CRSF_FRAMETYPE_DEVICE_PING = 0x28,
    CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
    CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
    CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
    CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
    CRSF_FRAMETYPE_ELRS_STATUS = 0x2E,
    CRSF_FRAMETYPE_COMMAND = 0x32,
    CRSF_FRAMETYPE_RADIO_ID = 0x3a,
    CRSF_FRAMETYPE_MSP_REQ = 0x7a,
    CRSF_FRAMETYPE_MSP_RESP = 0x7b,
    CRSF_FRAMETYPE_MSP_WRITE = 0x7c,
    CRSF_FRAMETYPE_DISPLAYPORT_CMD = 0x7d
} crsf_frametype_t;



typedef struct {
    UART_HandleTypeDef* huart;
}crsf_handle_t;

typedef struct __attribute__((packed)) {
    unsigned ch0 : 11;
    unsigned ch1 : 11;
    unsigned ch2 : 11;
    unsigned ch3 : 11;
    unsigned ch4 : 11;
    unsigned ch5 : 11;
    unsigned ch6 : 11;
    unsigned ch7 : 11;
    unsigned ch8 : 11;
    unsigned ch9 : 11;
    unsigned ch10 : 11;
    unsigned ch11 : 11;
    unsigned ch12 : 11;
    unsigned ch13 : 11;
    unsigned ch14 : 11;
    unsigned ch15 : 11;
}crsf_channels_t;



typedef struct{
    uint8_t address;
    uint8_t length;
    uint8_t payload[CRSF_MAX_PACKET_SIZE-6]; // 6 = address + length + crc
    uint8_t crc[2];
}crsf_channels_frame_t;





void crsf_init(crsf_handle_t * hcrsf, UART_HandleTypeDef *huart);
void crsf_process(crsf_handle_t * hcrsf, int16_t * data);
void crsf_send_frame_test(UART_HandleTypeDef *huart);
void crsf_tests(void);

