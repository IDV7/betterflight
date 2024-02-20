#include "mymain.h"
#include "gyro.h"
#include "main.h"
#include "misc.h"
#include <stdio.h>



void myinit(void)
{
}


void mymain(void)
{
        while (1) {
            //printf("Hello, World!\n");
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
            HAL_Delay(1000);
        }
}

