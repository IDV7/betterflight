/*
  This module contains misc functions that are used in multiple places in the code but don't fit in a specific place.
 */

#ifndef BETTERFLIGHT_MISC_H
#define BETTERFLIGHT_MISC_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

// callback function type
typedef void (*callback_t)(void *arg);

extern volatile uint32_t millis;
// toggles status led
void LED_toggle(void);

// turns on status led
void LED_on(void);

// turns off status led
void LED_off(void);

// sets status led to given state
void LED_set(bool state);

// LED_blink_pattern blinks the LED in a given pattern, saves space (lines of code)
void LED_blink_pattern(uint8_t count, int args_count, ...);

// none_blocking_delay has to be polled, and calls the given function (without parameters) when x ms have passed.
void none_blocking_delay(uint32_t ms, uint64_t *last_millis ,callback_t callback, void *arg);

// clean_str removes all \n and \r from a string
void clean_str(char *str);

// strcmp_ign (ign -> ignore) compares two strings, ignors \n and \r
int8_t strcmp_ign(const char *str1, const char *str2);

// char_to_uint16 converts a string to a uint16_t
uint16_t char_to_uint16(char *str);

// takes an 16 bit intiger and returns a string with the binary representation
uint8_t *byte_to_binary_str(uint16_t x);

void reboot_into_dfu();

// blocks for x ms (alias for HAL_Delay)
void delay(uint32_t ms);

// blocks for x us
void delay_us(uint16_t us);

// sets up the timer for delay_us
void setup_delay_us_tim();

// returns the current system clock speed
uint32_t get_mcu_clock_speed(void);

// blocks for x nop opperations
void delay_nop(uint32_t nops);


// maps a value from one range to another
int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max);

#endif //BETTERFLIGHT_MISC_H
