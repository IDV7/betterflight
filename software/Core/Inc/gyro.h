//
// Created by Isaak on 1/3/2024.
//

#ifndef BETTERFLIGHT_GYRO_H
#define BETTERFLIGHT_GYRO_H

#include <stdint-gcc.h>
#include "stm32f7xx_hal.h"

typedef enum {
    GYRO_OK = 0,
    GYRO_ERR_CHIP_ID = -1,
    GYRO_ERR_INTERNAL_STATUS = -3,
} gyro_err_t;


typedef struct
{
    uint8_t chip_id;
    uint8_t raw_data;
    uint8_t x;
    uint8_t y;
    uint8_t z;
} gyro_t;

gyro_err_t gyro_init(gyro_t *gyro);
gyro_err_t gyro_read(gyro_t *gyro);
void log_gyro_err_t(gyro_err_t err);

#endif //BETTERFLIGHT_GYRO_H
