#include "gyro.h"

#include <stdio.h>

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_spi.h"

#include "main.h"
#include "BMI270_CONFIG.h"
#include "log.h"

typedef enum {
    CHIP_ID = 0x00,
    PWR_CONF = 0x7C,
    INIT_CTRL = 0x59,
    INTERNAL_STATUS = 0x21,
    PWR_CTRL = 0x7D,
    ACC_CONF = 0x40,
    GYR_CONF = 0x42,
    DATA_8_TO_19 = 0x41,
    DATA_START = 0x0C,
} BMI270_REG_t;

#define NUM_BYTES_TO_READ 12

void gyro_cs_enable(void);
void gyro_cs_disable(void);
void gyro_send_spi(BMI270_REG_t reg, uint8_t *rx_data_ptr, uint8_t size);

gyro_err_t gyro_init(gyro_t *gyro)
{
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY); // wait for spi to be ready
    HAL_Delay(1000); // wait for gyro to power up

    //dummy read - configures the spi (its i2c by default)
    gyro_send_spi(CHIP_ID | 0x80, &gyro->chip_id, 1);

    delay(1000);

    // read chip id to verify communication
    gyro_send_spi(CHIP_ID | 0x80, &gyro->chip_id, 1);

    LOGD("chip id: %x\n", gyro->chip_id);
    if (gyro->chip_id != 0x24) {
        LOGE("gyro chip id not correct, gyro init not complete");
        return GYRO_ERR_CHIP_ID;
    }


    return GYRO_OK; //debugging
    // disable power save mode
    gyro_send_spi(PWR_CONF, 0x00, 1);
    HAL_Delay(1);

    // prepare config load
    gyro_send_spi(INIT_CTRL, 0x00, 1);


    //burst write to reg INIT_DATA Start with byte 0
    HAL_SPI_Transmit(&hspi1, (uint8_t *) bmi270_config_file, sizeof(bmi270_config_file), HAL_MAX_DELAY);
    // enable power save mode
    gyro_send_spi(PWR_CONF, (uint8_t*) 0x01, 1);

    // check internal status
    uint8_t status = 0;
    gyro_send_spi(INTERNAL_STATUS, &status, 1);
    if (status != 0x01) {
        gyro_cs_disable();
        return GYRO_ERR_INTERNAL_STATUS;
    }
    //enable acquisiton of acceleration; gyroscope and temperature sensor data; disable the auxiliary interface.
    gyro_send_spi(PWR_CTRL, (uint8_t*) 0x0E, 1);

    //enable the acc_filter_perf bit; set acc_bwp to normal mode; set acc_odr to 100 Hz
    gyro_send_spi(ACC_CONF, (uint8_t*) 0xA8, 1);

    //enable the gyr_filter_perf bit; enable; gyr_noise_perf bit; set gyr_bwp to normal mode; set gyr_odr to 200 Hz
    gyro_send_spi(GYR_CONF, (uint8_t*) 0xE9, 1);

    //disable the adv_power_save bit; leave the fifo_self_wakeup enabled
    gyro_send_spi(PWR_CONF, (uint8_t*) 0x02, 1);



    gyro_cs_disable();
    return GYRO_OK;
}

gyro_err_t gyro_read(gyro_t *gyro)
{   
    gyro_cs_enable();
    gyro_send_spi(DATA_START | 0x80, &gyro->raw_data, NUM_BYTES_TO_READ);
    gyro_cs_disable();
    return GYRO_OK;
}

void gyro_cs_enable(void)
{
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
}

void gyro_cs_disable(void)
{
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
}

void log_gyro_err_t(gyro_err_t err)
{
    switch (err) {
        case GYRO_OK:
            LOGE("[ERR] GYRO_OK");
            break;
        case GYRO_ERR_CHIP_ID:
            LOGE("[ERR] GYRO_ERR_CHIP_ID");
            break;
        case GYRO_ERR_INTERNAL_STATUS:
            LOGE("[ERR] GYRO_ERR_INTERNAL_STATUS");
            break;
        default:
            LOGE("[ERR] UNKNOWN GYRO_ERR");
            break;
    }
}

void gyro_send_spi(BMI270_REG_t reg, uint8_t *rx_data_ptr, uint8_t size) {
    uint8_t buffer[size+2];
    gyro_cs_enable();
    HAL_Delay(10);
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)reg, buffer, size+2, HAL_MAX_DELAY);
    gyro_cs_disable();
    for (uint8_t i = 0; i < size; i++) {
        rx_data_ptr[i] = buffer[i+2];
    }
//    for (uint8_t i = 0; i < size+2; i++) {
//        delay(250);
//        LOGD("rx_data_ptr[%d]: %x\n", i, rx_data_ptr[i]);
//        delay(250);
//    }
}