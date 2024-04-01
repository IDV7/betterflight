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
uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;

gyro_t gyro_h;
cli_handle_t cli_h;

void myinit(void) {
    cli_h.halt_until_connected_flag = true; //set to false if you don't want to wait for a connection

    log_init(LOG_LEVEL, true);
    LED_on();

    cli_init(&cli_h); //will block if halt_until_connected_flag is true

    LOGI("Starting Initialization...");
    // ----- all initialization code goes here ----- //

    gyro_init(&gyro_h);

    // ----- end initialization code ----- //
    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();
}


void mymain(void) {
    while (1) {
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
    }
}








