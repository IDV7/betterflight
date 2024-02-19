#include "mymain.h"
#include "gyro.h"
#include "main.h"
#include "misc.h"
#include <stdio.h>



void myinit(void)
{
    //just a test commit to test actions, test 3
}


void mymain(void)
{
        while (1) {
            printf("Hello, World!\n");
            LED_Toggle();
            HAL_Delay(1000);

        }
}

