

#include "misc.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "main.h"
#include "log.h"


volatile uint32_t millis = 0;

TIM_HandleTypeDef delay_timer_h;



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

// LED_blink_pattern blinks the LED in a given pattern, saves space (lines of code) */
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

// clean_str removes all \n and \r from a string
void clean_str(char *str) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        if (*src != '\n' && *src != '\r') {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

// strcmp_ign (ign -> ignore) compares two strings, ignors \n and \r
int8_t strcmp_ign(const char *str1, const char *str2) {
    // make copy to avoid modifying original strings
    char *copy1 = strdup(str1);
    char *copy2 = strdup(str2);

    // remove newlines and carriage returns
    clean_str(copy1);
    clean_str(copy2);

    int8_t result = (int8_t)strcmp(copy1, copy2);

    free(copy1);
    free(copy2);

    return result;
}

/*
  will write DEADBEEE on a known location in RAM, and reset the MCU
  it requires you to have code in the startup file to check for this magic data and if present goto the bootloader
 */
void reboot_into_dfu() {
    *((uint32_t *)0x20000000) = 0xDEADBEEE; // magic data that is checked for in startup code
    NVIC_SystemReset(); // reset mcu
}


// converts a string to a uint16_t
uint16_t char_to_uint16(char *str) {
    char *endptr;
    unsigned long num = strtoul(str, &endptr, 10);

    // Check for conversion errors
    if (endptr == str || *endptr != '\0' || errno == ERANGE || num > UINT16_MAX) {
        return 0; // Return 0 if conversion failed or number is out of range
    }

    return (uint16_t)num;
}

// gets the mcu clock speed
uint32_t get_mcu_clock_speed(void) {
    SystemCoreClockUpdate();
    return SystemCoreClock;
}

// takes an 16 bit intiger and returns a string with the binary representation
uint8_t* byte_to_binary_str(uint16_t x) {
    static uint8_t b[17]; //dirty static, was for debugging purposes and is not used anymore so not fixing in review
    // Ensure b is at least 17 characters long
    b[16] = '\0';

    for (int i = 15; i >= 0; i--) {
        b[15 - i] = (x & (1 << i)) ? '1' : '0';
    }

    return b;
}

// blocks for x ms (alias for HAL_Delay)
void delay(uint32_t ms) {
    HAL_Delay(ms);
}

// blocks for x nop opperations
void delay_nop(uint32_t nops) {
    while (nops--) {__NOP();}
}

// for setting up tim12 for delay_us
void setup_delay_us_tim() {
    __HAL_RCC_TIM12_CLK_ENABLE();
    delay_timer_h.Instance = TIM12;
    delay_timer_h.Init.Prescaler = 95;  // APB2 - 1
    delay_timer_h.Init.CounterMode = TIM_COUNTERMODE_UP;
    delay_timer_h.Init.Period = 0xFFFF;  //max period
    delay_timer_h.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    delay_timer_h.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&delay_timer_h); //hal init
}

// delay_us uses tim12 to delay for x us (inaccurate at very low us values, 1us -> ~3us, 10us -> ~11us, 100us -> ~100us)
void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&delay_timer_h , 0); //reset cnt
    HAL_TIM_Base_Start(&delay_timer_h);
    while (__HAL_TIM_GET_COUNTER(&delay_timer_h) < us) {}
    HAL_TIM_Base_Stop(&delay_timer_h);
}

// maps a value from one range to another
int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) {
    if(x < in_min) {
       return out_min;
    } else if(x > in_max) {
        return out_max;
    }
    return (int16_t )((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}