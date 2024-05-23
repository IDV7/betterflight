//
// Created by Isaak on 2/10/2024.
//


#include "misc.h"

#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>


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
void none_blocking_delay(uint32_t ms, uint64_t *last_millis, callback_t callback, void *arg) {
    if (*last_millis == 0) { // if not set, set last_millis
        *last_millis = millis;
    } else if ((millis - *last_millis) >= ms) { // if enough time has passed
        if (callback != NULL) { // if callback is valid
            callback(arg);
        }
        *last_millis = 0; // reset last_millis
    }
}


void clean_str(uint8_t *str) {
    uint8_t *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        if (*src != '\n' && *src != '\r') {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

// strcmp_ign (ign -> ignore) compares two strings, ignoring newlines and carriage returns
int8_t strcmp_ign(const uint8_t *str1, const uint8_t *str2) {
    // make copy to avoid modifying original strings
    uint8_t *copy1 = (uint8_t *)strdup((char *)str1);
    uint8_t *copy2 = (uint8_t *)strdup((char *)str2);

    // remove newlines and carriage returns
    clean_str(copy1);
    clean_str(copy2);

    int8_t result = (int8_t)strcmp((char *)copy1, (char *)copy2);

    free(copy1);
    free(copy2);

    return result;
}

/* will write DEADBEEE on a known location in RAM, and reset the MCU */
/* it requires you to have code in the startup file to check for this magic data and if present goto the bootloader */
void reboot_into_dfu() {
    *((uint32_t *)0x20000000) = 0xDEADBEEE; // magic data that is checked for in startup code
    NVIC_SystemReset(); // reset mcu
}

void delay(uint32_t ms) {
    HAL_Delay(ms);
}

// 1000us = 970 actual us, 10us = 15 actual us
void delay_us(uint32_t us) {

    /* //not working
    //uint32_t mcu_clock_speed = get_mcu_clock_speed();
    uint32_t startTick = SysTick->VAL; // current tick count
    uint32_t ticks = (192000000 / 1000000) * us; //tick count for us delay
    while ((SysTick->VAL - startTick) < ticks);
    */

    float nops_per_us = 5.56; //calculated using logic analyzer
    float total_nops = (float)us * nops_per_us;

    uint32_t nops_to_execute = (uint32_t)ceilf(total_nops);

    delay_nop(nops_to_execute);
}

uint16_t char_to_uint16(char *str) {
    char *endptr;
    unsigned long num = strtoul(str, &endptr, 10);

    // Check for conversion errors
    if (endptr == str || *endptr != '\0' || errno == ERANGE || num > UINT16_MAX) {
        return 0; // Return 0 if conversion failed or number is out of range
    }

    return (uint16_t)num;
}

uint32_t get_mcu_clock_speed(void) {
    SystemCoreClockUpdate();
    return SystemCoreClock;
}

// takes an 16 bit intiger and returns a string with the binary representation
uint8_t* byte_to_binary_str(uint16_t x) {
    static uint8_t b[17];
    // Ensure b is at least 17 characters long
    b[16] = '\0';

    for (int i = 15; i >= 0; i--) {
        b[15 - i] = (x & (1 << i)) ? '1' : '0';
    }

    return b;
}

__attribute__((optimize("O0")))  void delay_nop(uint32_t nops) {
    while (nops--) {__NOP();}
}