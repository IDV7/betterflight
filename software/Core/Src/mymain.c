#include "mymain.h"

#include <stdio.h>

#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"


void myinit(void)
{
    log_init(LOG_LEVEL_DEBUG, true);
    LOGI("Starting Initialization...");
    LED_on();


    LOGI("Finished Initialization");
    for (int i = 0; i < 20; i++) {
        HAL_Delay(50);
        LED_toggle();
    }
    LED_off();
}


void mymain(void) {
    while (1) {

        LOGD("Hello from mymain!");

        HAL_Delay(1000);
        LED_toggle();
    }
}