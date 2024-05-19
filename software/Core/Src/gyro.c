#include "gyro.h"

#include "stm32f7xx_hal.h"

#include "main.h"
#include "log.h"
#include "spi_soft.h"

#include "BMI270_CONFIG.h"

typedef enum {
    REG_CHIP_ID = 0x00,
    REG_CMD_REG= 0x7E,
    REG_PWR_CONF = 0x7C,
    REG_INIT_CTRL = 0x59,
    REG_INTERNAL_STATUS = 0x21,
    REG_PWR_CTRL = 0x7D,
    REG_ACC_CONF = 0x40,
    REG_GYR_CONF = 0x42,
    REG_DATA_8_TO_19 = 0x41,
    REG_DATA_START = 0x0C,
} BMI270_REG_t;

#define NUM_BYTES_TO_READ 12
#define READ_ADDR(x) (x | 0x80)

uint8_t gyro_read_reg(gyro_handle_t *gyro_h, uint8_t reg);
void gyro_write_reg(gyro_handle_t *gyro_h, uint8_t reg, uint8_t data);
void gyro_burst_write(gyro_handle_t *gyro_h, uint8_t reg, const uint8_t *data, uint8_t len);

gyro_err_t gyro_init(gyro_handle_t *gyro_h) {

    gyro_h->spi_h.sck.port = GPIOA;
    gyro_h->spi_h.sck.pin = GPIO_PIN_5;

    gyro_h->spi_h.miso.port = GPIOA;
    gyro_h->spi_h.miso.pin = GPIO_PIN_6;

    gyro_h->spi_h.mosi.port = GPIOA;
    gyro_h->spi_h.mosi.pin = GPIO_PIN_7;

    gyro_h->spi_h.cs.port = GPIOB;
    gyro_h->spi_h.cs.pin = GPIO_PIN_2;

    SPI_soft_init(&gyro_h->spi_h);

    delay(1);

    // set the CS low and high quickly to activate(?) the BMI270...?
    SPI_soft_cs_low(&gyro_h->spi_h); // set cs low
    delay(1);
    SPI_soft_cs_high(&gyro_h->spi_h); // set cs high
    delay(50);

    // check for chip id (0x24)
    uint8_t chip_id_tx_buffer[] = {READ_ADDR(REG_CHIP_ID), 0x00, 0x00};
    uint8_t chip_id_rx_buffer[] = {0x00, 0x00, 0x00};
    delay(10);
    SPI_soft_trx(&gyro_h->spi_h, chip_id_tx_buffer, chip_id_rx_buffer, 3);
    if (chip_id_rx_buffer[2] != 0x24) {
        return GYRO_ERR_CHIP_ID;
    }

    delay(1);
    gyro_write_reg(gyro_h, REG_CMD_REG, 0x6B); //soft reset
    delay(10);
    gyro_write_reg(gyro_h, REG_PWR_CONF, 0x00); // set power config to 0x00 (advanced power save disable)
    delay(1);

    // prepare config load
    gyro_write_reg(gyro_h, REG_INIT_CTRL, 0x00);
    delay(1);

    // burst write to reg INIT_DATA Start with byte 0
    gyro_burst_write(gyro_h, REG_DATA_START, BMI_270_CONF_DATA_ARRAY, (uint8_t) sizeof(BMI_270_CONF_DATA_ARRAY));


    return GYRO_OK;
}

uint8_t gyro_read_reg(gyro_handle_t *gyro_h, uint8_t reg) {

    uint8_t tx_buffer[] = {(reg | 0x80), 0x00}; // | 0x80 to set the read bit
    uint8_t rx_buffer[] = {0x00, 0x00};

    SPI_soft_trx(&gyro_h->spi_h, tx_buffer, rx_buffer, 2);

    return rx_buffer[1];
}

void gyro_write_reg(gyro_handle_t *gyro_h, uint8_t reg, uint8_t data) {

    uint8_t tx_buffer[] = {reg, data};
    uint8_t rx_buffer[] = {0x00, 0x00};

    SPI_soft_trx(&gyro_h->spi_h, tx_buffer, rx_buffer, 2);
}

void gyro_burst_write(gyro_handle_t *gyro_h, uint8_t reg, const uint8_t *data, uint8_t arr_len) {

    uint8_t tx_buffer[arr_len + 1];
    tx_buffer[0] = reg;

    for (int i = 0; i < arr_len; i++) {
        tx_buffer[i + 1] = data[i];
    }

    SPI_soft_burst_tx(&gyro_h->spi_h, tx_buffer, arr_len + 1);

}


void log_gyro_err_t(gyro_err_t err) {
    switch (err) {
        case GYRO_OK:
            LOGI("GYRO_OK");
            break;
        case GYRO_ERR_CHIP_ID:
            LOGE("GYRO_ERR_CHIP_ID");
            break;
        case GYRO_ERR_INTERNAL_STATUS:
            LOGE("GYRO_ERR_INTERNAL_STATUS");
            break;
        default:
            LOGE("UNKNOWN GYRO_ERR");
            break;
    }
}