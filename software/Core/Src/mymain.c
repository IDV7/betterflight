#include "mymain.h"

#include <stdio.h>

#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include "cli.h"
#include "dshot.h"
#include "motors.h"


/* SETTINGS */
#define LOG_LEVEL LOG_LEVEL_DEBUG
// check version.h for version settings

/* EOF SETTINGS */

gyro_t gyro_h;
cli_handle_t cli_h;

//motors
dshot_handle_t m_hs[4];
motors_handle_t *motors_h;

uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;
uint64_t motors_process_last_ms = 0;


void myinit(void) {
    cli_h.halt_until_connected_flag = true; //set to false if you don't want to wait for a connection

    log_init(LOG_LEVEL, true);
    LED_on();

    cli_init(&cli_h); //will block if halt_until_connected_flag is true

    delay(1);
    LOGI("Starting Initialization...");
    // ----- all initialization code goes here ----- //

    //gyro_init(&gyro_h);
    dshot_init(&m_hs[0], &htim1, &hdma_tim1_ch2, TIM_CHANNEL_2);
    dshot_init(&m_hs[1], &htim1, &hdma_tim1_ch1, TIM_CHANNEL_1);
    dshot_init(&m_hs[2], &htim8, &hdma_tim8_ch4_trig_com, TIM_CHANNEL_4);
    dshot_init(&m_hs[3], &htim8, &hdma_tim8_ch3, TIM_CHANNEL_3);
    motors_init(motors_h, m_hs[0], m_hs[1], m_hs[2], m_hs[3]);
    // ----- end initialization code ----- //

    delay(1);
    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();

}

void mymain(void) {
    while (1) {
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
        none_blocking_delay(1, &motors_process_last_ms, (callback_t) motors_process, &motors_h);
    }
}








