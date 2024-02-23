//
// Created by Isaak on 2/10/2024.
//


#include "misc.h"

#include <stdarg.h>
#include <malloc.h>

#include "main.h"
#include "log.h"

volatile uint32_t millis = 0;

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

// TODO: potentially buggy, but works for now (I think it reverses the order of the pattern)
/* LED_blink_pattern blinks the LED in a given pattern, saves space (lines of code) */
void LED_blink_pattern(uint8_t count, int num_durations, ...) {
    va_list durations;
    va_start(durations, num_durations);
    uint16_t * duration_array = (uint16_t*)malloc(num_durations * sizeof(uint16_t));
    if (duration_array == NULL) {
        LOGE("Failed to allocate memory for duration_array");
        return;
    }
    for (uint8_t i = 0; i < num_durations; i++) {
        duration_array[i] = va_arg(durations, int);
    }
    va_end(durations);
    LED_off(); // turn it off so first blink is on
    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t j = 0; j < (num_durations - 1); j++) {
            LED_toggle();
            HAL_Delay(duration_array[j]);
        }
    }
    LED_off(); // for safety...
    free(duration_array);
}

// none_blocking_delay has to be polled, and calls the given function (without parameters) when x ms have passed.
void none_blocking_delay(uint32_t ms, callback_t callback, uint64_t *last_millis) {
    if (*last_millis == 0) { // if not set, set last_millis
        *last_millis = millis;
    } else if ((millis - *last_millis) >= ms) { // if enough time has passed
        if (callback != NULL) { // if callback is valid
            callback(NULL);
        }
        *last_millis = 0; // reset last_millis
    }
}