#include "mymain.h"
#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include <stdio.h>



void myinit(void)
{
    log_init(LOG_LEVEL_DEBUG, true);
    LOGI("Initializing...");
    LED_on();

    LOGI("Initialized.");
    for (int i = 0; i < 20; i++) {
        HAL_Delay(50);
        LED_toggle();
    }
    LED_off();
}


void mymain(void)
{
    uint8_t buffer[]="Hello World!";
    while (1) {
        LOGD("Hello World!");
        HAL_Delay(1000);
        LED_toggle();
    }
}

