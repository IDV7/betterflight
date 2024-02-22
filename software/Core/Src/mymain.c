#include "mymain.h"
#include "gyro.h"
#include "main.h"
#include "misc.h"
#include "log.h"
#include <stdio.h>


void myinit(void)
{
    printf("Hello World!\n");
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


void mymain(void)
{
    while (1) {
        uint32_t brr_value = huart2.Instance->BRR;
        LOGD("BRR: %d\n", brr_value);
        LOGD("Test value: %d\n", 1234);
        HAL_Delay(1000);
        LED_toggle();
    }
}

