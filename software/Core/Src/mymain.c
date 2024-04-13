#include "mymain.h"

#include <stdio.h>

#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include "cli.h"
#include "dshot.h"

/* SETTINGS */
#define LOG_LEVEL LOG_LEVEL_DEBUG
// check version.h for version settings

/* EOF SETTINGS */


gyro_t gyro_h;
cli_handle_t cli_h;

//motors
dshot_handle_t hm1; // TIM1 CH2
dshot_handle_t hm2; // TIM1 CH1
dshot_handle_t hm3; // TIM8 CH4
dshot_handle_t hm4; // TIM8 CH3

uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;


void myinit(void) {
    cli_h.halt_until_connected_flag = true; //set to false if you don't want to wait for a connection

    log_init(LOG_LEVEL, true);
    LED_on();

    cli_init(&cli_h); //will block if halt_until_connected_flag is true

    LOGI("Starting Initialization...");
    // ----- all initialization code goes here ----- //

    gyro_init(&gyro_h);
    dshot_init(&hm2, &htim1, &hdma_tim1_ch1, TIM_CHANNEL_1);
    // ----- end initialization code ----- //

    delay(1);
    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();

    LOGD("sending dshot value 0:");
    dshot_send_throttle(&hm2, 0);
    delay(2000);
    LOGD("sending dshot value 1250:");
    dshot_send_throttle(&hm2, 1250);
}

// none blocking loop funciton
void dshot_test(void) {
    static uint16_t motor_value_last = 0;



    if (motor_value_last < DSHOT_MAX_THROTTLE) {
        motor_value_last += 50;
    } else {
        motor_value_last = DSHOT_MIN_THROTTLE;
    }
    LOGD("Sending motor value: %d", motor_value_last);
    dshot_send(&hm2, &motor_value_last);
}

void mymain(void) {
    while (1) {
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
        //none_blocking_delay(500, NULL, (callback_t) dshot_test, NULL);
    }
}








