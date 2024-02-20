//
// Created by Isaak on 2/10/2024.
//


#include "misc.h"
#include "main.h"

void LED_toggle(void)
{
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

void LED_on(void)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

void LED_off(void)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

void LED_set(bool state)
{
    if (state)
        LED_on();
    else
        LED_off();
}