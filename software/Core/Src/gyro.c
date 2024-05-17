#include "gyro.h"


#include "stm32f7xx_hal.h"

#include "main.h"
#include "log.h"
#include "spi_soft.h"
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

void gyro_send_spi(BMI270_REG_t reg, uint8_t *rx_data_ptr, uint8_t size);

gyro_err_t gyro_init(gyro_handle_t *gyro_h) {

    gyro_h->spi_h.miso.port = SPI_MISO_GPIO_Port;
    gyro_h->spi_h.miso.pin = SPI_MISO_Pin;

    gyro_h->spi_h.mosi.port = SPI_MOSI_GPIO_Port;
    gyro_h->spi_h.mosi.pin = SPI_MOSI_Pin;

    gyro_h->spi_h.sck.port = SPI_SCK_GPIO_Port;
    gyro_h->spi_h.sck.pin = SPI_SCK_Pin;

    gyro_h->spi_h.cs.port = GYRO_CS_GPIO_Port;
    gyro_h->spi_h.cs.pin = GYRO_CS_Pin;

    SPI_Init(&gyro_h->spi_h, 6000000);  // spi speed = 6 MHz => MCU speed/32 => 192MHz/32 = 6 MHz
    HAL_Delay(500); //temp
    uint8_t tx_buffer[] = {0x80, 0x00, 0x00};
    uint8_t rx_buffer[] = {0x00, 0x00, 0x00};

    SPI_transmit_receive(&gyro_h->spi_h, tx_buffer, rx_buffer, 3);
    //SPI_trx_deb(&gyro_h->spi_h);


    return GYRO_OK;
}

void log_gyro_err_t(gyro_err_t err) {
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