//
// Created by Isaak on 2/10/2024.
//

#ifndef BETTERFLIGHT_MISC_H
#define BETTERFLIGHT_MISC_H

#include <stdint.h>
#include <stdarg.h>

typedef enum {false = 0, true = 1} bool;
typedef void (*callback_t)(void *arg);

void LED_toggle(void);
void LED_on(void);
void LED_off(void);
void LED_set(bool state);
void LED_blink_pattern(uint8_t count, int args_count, ...);

void none_blocking_delay(uint32_t ms, callback_t callback, uint64_t *last_millis);

void clean_str(uint8_t *str);
int8_t strcmp_ign(const uint8_t *str1, const uint8_t *str2);


#endif //BETTERFLIGHT_MISC_H
