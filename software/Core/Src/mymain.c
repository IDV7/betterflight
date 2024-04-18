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
dshot_handle_t m1_h; // TIM1 CH2
dshot_handle_t m2_h; // TIM1 CH1
dshot_handle_t m3_h; // TIM8 CH4
dshot_handle_t m4_h; // TIM8 CH3

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
    dshot_init(&m1_h, &htim1, &hdma_tim1_ch2, TIM_CHANNEL_2);
    // ----- end initialization code ----- //

    delay(1);
    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();

    LOGD("sending dshot value 0 to init esc");
    dshot_send_throttle(&m1_h, 0);
    delay(2000);
}

// none blocking loop funciton
void dshot_test(void) {
    static uint16_t motor_value_last = DSHOT_MIN_THROTTLE;

    if (motor_value_last < (DSHOT_MAX_THROTTLE / 3)) {
        motor_value_last += 50;
    } else {
        motor_value_last = DSHOT_MIN_THROTTLE;
    }
    LOGD("Sending motor value: %d", motor_value_last);
    dshot_send(&m1_h, &motor_value_last);
}


void mymain(void) {
    while (1) {
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
        none_blocking_delay(1500, NULL, (callback_t) dshot_test, NULL);
    }
}








