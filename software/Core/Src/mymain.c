#include "mymain.h"

#include <stdio.h>

#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include "cli.h"
#include "pid_controller.h"
/* SETTINGS */
#define LOG_LEVEL LOG_LEVEL_DEBUG
// check version.h for version settings

/* EOF SETTINGS */
uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;
uint64_t pid_controller_test_ms = 0;
gyro_t gyro_h;
cli_handle_t cli_h;

void myinit(void) {
    log_init(LOG_LEVEL, true);
    if (get_log_level() == LOG_LEVEL_DEBUG) // give dev time to open serial monitor when debugging
        LED_blink_pattern(5, 4 ,50, 75, 50, 825);

    LOGI("Starting Initialization...");
    LED_on();

    cli_init(&cli_h);

    LOGI("Finished Initialization");


    LED_blink_pattern(20, 2, 50, 50);
    LED_off();
}


void mymain(void) {
    while (1) {
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
        none_blocking_delay(1000, &pid_controller_test_ms, (callback_t) test_pid_controller, NULL);
    }
}

