//
// Created by Isaak on 2/10/2024.
//

#ifndef BETTERFLIGHT_MISC_H
#define BETTERFLIGHT_MISC_H

typedef enum {false = 0, true = 1} bool;

void LED_toggle(void);
void LED_on(void);
void LED_off(void);
void LED_set(bool state);

#endif //BETTERFLIGHT_MISC_H
