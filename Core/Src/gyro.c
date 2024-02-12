#include "gyro.h"
#include "BMI270_CONFIG.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_spi.h"
#include <stdio.h>


#define BMI270_CHIP_ID_REG 0x00
#define BMI270_PWR_CONF_REG 0x7C
#define BMI270_INIT_CTRL_REG 0x59
#define BMI270_INTERNAL_STATUS_REG 0x21
#define BMI270_PWR_CTRL_REG 0x7D
#define BMI270_ACC_CONF_REG 0x40
#define BMI270_GYR_CONF_REG 0x42
#define BMI270_DATA_8_TO_19 0x41
#define BMI270_DATA_START_REG 0x0C
#define NUM_BYTES_TO_READ 12

void gyro_cs_enable(void);
void gyro_cs_disable(void);

gyro_err_t gyro_init(gyro_t *gyro)
{
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY); // wait for spi to be ready
    HAL_Delay(1000); // wait for gyro to power up
    gyro_cs_enable(); // enable chip select

    //dummy read
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_CHIP_ID_REG, NULL, 1, HAL_MAX_DELAY);

    // read chip id to init spi on the BMI270
    HAL_SPI_TransmitReceive(&hspi1, BMI270_CHIP_ID_REG, &gyro->chip_id, 1, HAL_MAX_DELAY);
    printf("[debug] chip id: %x\n", gyro->chip_id);
    if (gyro->chip_id != 0x24) {
        gyro_cs_disable();
        return GYRO_ERR_CHIP_ID;
    }

    // disable power save mode
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_PWR_CONF_REG, 0x00, 1, HAL_MAX_DELAY);
    HAL_Delay(1);

    // prepare config load
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_INIT_CTRL_REG, 0x00, 1, HAL_MAX_DELAY);


    //burst write to reg INIT_DATA Start with byte 0
    HAL_SPI_Transmit(&hspi1, (uint8_t *) bmi270_config_file, sizeof(bmi270_config_file), HAL_MAX_DELAY);

    // enable power save mode
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_PWR_CONF_REG, (uint8_t*) 0x01, 1, HAL_MAX_DELAY);

    // check internal status
    uint8_t status = 0;
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_INTERNAL_STATUS_REG, &status, 1, HAL_MAX_DELAY);
    if (status != 0x01) {
        gyro_cs_disable();
        return GYRO_ERR_INTERNAL_STATUS;
    }
    //enable acquisiton of acceleration; gyroscope and temperature sensor data; disable the auxiliary interface.
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_PWR_CTRL_REG, (uint8_t*) 0x0E, 1, HAL_MAX_DELAY);

    //enable the acc_filter_perf bit; set acc_bwp to normal mode; set acc_odr to 100 Hz
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_ACC_CONF_REG, (uint8_t*) 0xA8, 1, HAL_MAX_DELAY);

    //enable the gyr_filter_perf bit; enable; gyr_noise_perf bit; set gyr_bwp to normal mode; set gyr_odr to 200 Hz
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_GYR_CONF_REG, (uint8_t*) 0xE9, 1, HAL_MAX_DELAY);

    //disable the adv_power_save bit; leave the fifo_self_wakeup enabled
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) BMI270_PWR_CONF_REG, (uint8_t*) 0x02, 1, HAL_MAX_DELAY);



    gyro_cs_disable();
    return GYRO_OK;
}

gyro_err_t gyro_read(gyro_t *gyro)
{   
    gyro_cs_enable();
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) (BMI270_DATA_START_REG | 0x80), &gyro->raw_data, NUM_BYTES_TO_READ, HAL_MAX_DELAY);
    gyro_cs_disable();
    return GYRO_OK;
}

void gyro_cs_enable(void)
{
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
    printf("[debug] gyro cs enabled\n");
}

void gyro_cs_disable(void)
{
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
    printf("[debug] gyro cs disabled\n");
}

void print_gyro_err_t (gyro_err_t err)
{
    switch (err) {
        case GYRO_OK:
            printf("[ERR] GYRO_OK\n");
            break;
        case GYRO_ERR_CHIP_ID:
            printf("[ERR] GYRO_ERR_CHIP_ID\n");
            break;
        case GYRO_ERR_INTERNAL_STATUS:
            printf("[ERR] GYRO_ERR_INTERNAL_STATUS\n");
            break;
        default:
            printf("[ERR] UNKNOWN GYRO_ERR\n");
            break;
    }
}