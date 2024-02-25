#include "mymain.h"

#include <stdio.h>

#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include "cli.h"

/* SETTINGS */
#define LOG_LEVEL LOG_LEVEL_DEBUG
// check version.h for version settings

/* EOF SETTINGS */

extern uint8_t cli_rx_buffer[64];

uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;

void myinit(void) {
    log_init(LOG_LEVEL, true);
    if (get_log_level() == LOG_LEVEL_DEBUG) { // give dev time to open serial monitor when debugging
        LED_blink_pattern(5, 4 ,50, 75, 50, 825);
    }

    LOGI("Starting Initialization...");
    LED_on();



    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();
}


void mymain(void) {
    while (1) {
        none_blocking_delay(1000, (callback_t) LED_toggle, &led_toggle_last_ms);
        none_blocking_delay(25, (callback_t) cli_process, &cli_process_last_ms);
    }
}

/* dev callbacks for testing purpose, type devx in the cli and the corresponding function will be called */

void dev1_callback(void) {
}

void dev2_callback(void) {
}

void dev3_callback(void) {
}

