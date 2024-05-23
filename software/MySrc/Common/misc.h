//
// Created by Isaak on 2/10/2024.
//

#ifndef BETTERFLIGHT_MISC_H
#define BETTERFLIGHT_MISC_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

typedef void (*callback_t)(void *arg);

extern volatile uint32_t millis;

void LED_toggle(void);
void LED_on(void);
void LED_off(void);
void LED_set(bool state);
void LED_blink_pattern(uint8_t count, int args_count, ...);

void none_blocking_delay(uint32_t ms, uint64_t *last_millis ,callback_t callback, void *arg);

void clean_str(uint8_t *str);
int8_t strcmp_ign(const uint8_t *str1, const uint8_t *str2);

void reboot_into_dfu();

void delay(uint32_t ms);
void delay_us(uint32_t us);

uint16_t char_to_uint16(char *str);

uint32_t get_mcu_clock_speed(void);

uint8_t *byte_to_binary_str(uint16_t x);

#endif //BETTERFLIGHT_MISC_H
