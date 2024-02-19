#include "mymain.h"
#include "gyro.h"
#include "main.h"
#include "misc.h"
#include <stdio.h>



void myinit(void)
{
    //just sdfa test commit to test actions, test 4
}


void mymain(void)
{
        while (1) {
            printf("Hello, World!\n");
            LED_Toggle();
            HAL_Delay(1000);

        }
}

