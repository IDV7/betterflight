#ifndef BETTERFLIGHT_DSHOT_H
#define BETTERFLIGHT_DSHOT_H

#include "main.h"
#include "misc.h"


#define MOTOR_BIT_0 240 // ~33% duty cycle
#define MOTOR_BIT_1 480 // ~66% duty cycle
#define DSHOT_PSC 0 // prescaler
#define DSHOT_ARR 639 // auto reload register

#define DSHOT_FRAME_SIZE 16
#define DSHOT_DMA_BUFFER_SIZE 18 // 16 data bits + 2 stop bits (reset)

#define DSHOT_MIN_THROTTLE 48
#define DSHOT_MAX_THROTTLE 2047
#define DSHOT_RANGE (DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

#define CMD_ONLY_WHILE_STOP_RANGE 36 // only allow special commands to be send when motors if they are from 0 to 36




//dshot special commands, send these values to the motor instead of the throttle value to execute the command
typedef enum {
    DSHOT_CMD_BEEP1 = 1, // Wait at least length of beep (260ms) before next command
    DSHOT_CMD_BEEP2 = 2, // Wait at least length of beep (260ms) before next command
    DSHOT_CMD_BEEP3 = 3, // Wait at least length of beep (260ms) before next command
    DSHOT_CMD_BEEP4 = 4, // Wait at least length of beep (260ms) before next command
    DSHOT_CMD_BEEP5 = 5, // Wait at least length of beep (260ms) before next command
    DSHOT_CMD_ESC_INFO = 6, // Wait at least 12ms before next command
    DSHOT_CMD_SPIN_DIRECTION_1 = 7, // Need 6x
    DSHOT_CMD_SPIN_DIRECTION_2 = 8, // Need 6x
    DSHOT_CMD_3D_MODE_OFF = 9, // Need 6x
    DSHOT_CMD_3D_MODE_ON = 10, // Need 6x
    DSHOT_CMD_SETTINGS_REQUEST = 11, // Currently not implemented
    DSHOT_CMD_SAVE_SETTINGS = 12, // Need 6x, wait at least 35ms before next command
    DSHOT_EXTENDED_TELEMETRY_ENABLE = 13, // Need 6x (only on EDT enabled firmware)
    DSHOT_EXTENDED_TELEMETRY_DISABLE = 14, // Need 6x (only on EDT enabled firmware)
    DSHOT_CMD_SPIN_DIRECTION_NORMAL = 20, // Need 6x
    DSHOT_CMD_SPIN_DIRECTION_REVERSED = 21, // Need 6x
    DSHOT_CMD_LED0_ON = 22,
    DSHOT_CMD_LED1_ON = 23,
    DSHOT_CMD_LED2_ON = 24,
    DSHOT_CMD_LED3_ON = 25,
    DSHOT_CMD_LED0_OFF = 26,
    DSHOT_CMD_LED1_OFF = 27,
    DSHOT_CMD_LED2_OFF = 28,
    DSHOT_CMD_LED3_OFF = 29,
    DSHOT_CMD_SIGNAL_LINE_TELEMETRY_DISABLE = 32, // Need 6x. Disables commands 42 to 47
    DSHOT_CMD_SIGNAL_LINE_TELEMETRY_ENABLE = 33, // Need 6x. Enables commands 42 to 47
    DSHOT_CMD_SIGNAL_LINE_CONTINUOUS_ERPM_TELEMETRY = 34, // Need 6x. Enables commands 42 to 47 and sends erpm if normal Dshot frame
    DSHOT_CMD_SIGNAL_LINE_CONTINUOUS_ERPM_PERIOD_TELEMETRY = 35, // Need 6x. Enables commands 42 to 47 and sends erpm period if normal Dshot frame
    DSHOT_CMD_SIGNAL_LINE_TEMPERATURE_TELEMETRY = 42, // 1°C per LSB
    DSHOT_CMD_SIGNAL_LINE_VOLTAGE_TELEMETRY = 43, // 10mV per LSB, 40.95V max
    DSHOT_CMD_SIGNAL_LINE_CURRENT_TELEMETRY = 44, // 100mA per LSB, 409.5A max
    DSHOT_CMD_SIGNAL_LINE_CONSUMPTION_TELEMETRY = 45, // 10mAh per LSB, 40.95Ah max
    DSHOT_CMD_SIGNAL_LINE_ERPM_TELEMETRY = 46, // 100erpm per LSB, 409500erpm max
    DSHOT_CMD_SIGNAL_LINE_ERPM_PERIOD_TELEMETRY = 47 // 16us per LSB, 65520us max TBD
} dshot_cmd_t;

// struct to keep track of special command send count
typedef struct {
    dshot_cmd_t cmd;
    int8_t send_count;
    uint16_t delayms_after_cmd;
} dshot_cmd_info_t;

// table to keep track of special command info
extern const dshot_cmd_info_t DSHOT_CMD_SEND_INFO[];

typedef struct {
    uint32_t dma_buffer[DSHOT_DMA_BUFFER_SIZE];
    TIM_HandleTypeDef *htim;
    DMA_HandleTypeDef *hdma;
    uint32_t tim_channel;
    uint16_t throttle_value; // last speed value set by user (altered by dshot_set_throttle!!)
    dshot_cmd_info_t *cmd_cnts; //cmd and its counters (note: cmd_cnts->send_count = -1 means cmd timeout)
} dshot_handle_t;

void dshot_init(dshot_handle_t *dshot_h, TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma, uint32_t tim_channel);
void dshot_process(dshot_handle_t *dshot_h);
void dshot_set_throttle(dshot_handle_t *dshot_h, uint16_t throttle);
void dshot_stop(dshot_handle_t *dshot_h);
void dshot_send_special_command(dshot_handle_t *dshot_h, dshot_cmd_t cmd);

#endif //BETTERFLIGHT_DSHOT_H
