#include "gyro.h"

#include "stm32f7xx_hal.h"

#include "main.h"
#include "log.h"
#include "spi_soft.h"

#include "BMI270_CONFIG.h"
#include "bmi270.h"


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

typedef enum {  // Register 0x21 INTERNAL_STATUS -> see BMI270 datasheet
    INT_STATUS__NOT_INIT = 0x00, // ASIC not initialized
    INT_STATUS__INIT_OK = 0x01, // ASIC initialized
    INT_STATUS__INIT_ERR = 0x02, // Initialization error
    INT_STATUS__DRV_ERR = 0x03, // Invalid driver
    INT_STATUS__SNS_STOP = 0x04, // Sensor stopped
    INT_STATUS__NVM_ERROR = 0x05, //Internal error while accessing NVM
    INT_STATUS__START_UP_ERROR = 0x06, // Internal error while accessing NVM and initialization error
    INT_STATUS__COMPAT_ERROR = 0x07, // Compatibility error
    INT_STATUS__AXES_REMAP_ERROR = 0x20, // Incorrect axes remapping, XYZ axes must be mapped to exclusively separate axes i.e. they cannot be mapped to same axis.
    INT_STATUS__ODR_50HZ_ERROR = 0x30, // The minimum bandwidth conditions are not respected for the features which require 50 Hz data.
    INT_STATUS__UNKNOWN = 0xFF,
} BMI270_INT_STATUS_ERR_t;


#define NUM_BYTES_TO_READ 12
#define READ_ADDR(x) (x | 0x80)

uint8_t gyro_read_reg(gyro_handle_t *gyro_h, uint8_t reg);
void gyro_write_reg(gyro_handle_t *gyro_h, uint8_t reg, uint8_t data);
void gyro_burst_write(gyro_handle_t *gyro_h, uint8_t reg, const uint8_t *data, uint32_t len);
BMI270_INT_STATUS_ERR_t parse_internal_status_err(uint8_t reg_dump);
void log_gyro_int_status_err(BMI270_INT_STATUS_ERR_t err);

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

//    delay(1);
//    gyro_write_reg(gyro_h, REG_CMD_REG, 0x6B); //soft reset
    delay(10);
    gyro_write_reg(gyro_h, REG_PWR_CONF, 0x00); // set power config to 0x00 (advanced power save disable)
    delay(1);

    // prepare config load
    gyro_write_reg(gyro_h, REG_INIT_CTRL, 0x00);
    delay(1);

    // burst write to reg INIT_DATA Start with byte 0
    gyro_burst_write(gyro_h, REG_DATA_START, BMI_270_CONF_DATA_ARRAY, (uint32_t ) sizeof(BMI_270_CONF_DATA_ARRAY));

    delay(10);

    gyro_write_reg(gyro_h, REG_INIT_CTRL, 0x01); // complete config load

    delay(1);

    // check internal status
    uint8_t internal_status = gyro_read_reg(gyro_h, REG_INTERNAL_STATUS);
    if (parse_internal_status_err(internal_status) != INT_STATUS__INIT_OK) {
        log_gyro_int_status_err(parse_internal_status_err(internal_status));
        delay(10); // RWBA (remove when buffer added)
        LOGE("internal status error reg dump (0x21): %s", byte_to_binary_str(internal_status));
        return GYRO_ERR_INTERNAL_STATUS;
    }


    return GYRO_OK;
}

uint8_t gyro_read_reg(gyro_handle_t *gyro_h, uint8_t reg) {

    uint8_t tx_buffer[] = {(reg | 0x80), 0x00, 0x00}; // | 0x80 to set the read bit
    uint8_t rx_buffer[] = {0x00, 0x00, 0x00};

    SPI_soft_trx(&gyro_h->spi_h, tx_buffer, rx_buffer, 3);

    return rx_buffer[2];
}

void gyro_write_reg(gyro_handle_t *gyro_h, uint8_t reg, uint8_t data) {

    uint8_t tx_buffer[] = {reg, data};
    uint8_t rx_buffer[] = {0x00, 0x00};

    SPI_soft_trx(&gyro_h->spi_h, tx_buffer, rx_buffer, 2);
}

void gyro_burst_write(gyro_handle_t *gyro_h, uint8_t reg, const uint8_t *data, uint32_t arr_len) {

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


BMI270_INT_STATUS_ERR_t parse_internal_status_err(uint8_t reg_dump) {
    if (!(reg_dump & 0x01)) { // if INT_STATUS__INIT_OK bit is not set
        if ((reg_dump & 0x0F) > 0x07) { //check if the value is possible
            return INT_STATUS__UNKNOWN;
        }
        return (BMI270_INT_STATUS_ERR_t) (reg_dump & 0x0F); // return the INTERNAL_STATUS error (message) (cut of the last 4 bits)
    }

    // check if bit [4-7] are set and return the corresponding error
    if (reg_dump & 0x10) {
        return INT_STATUS__AXES_REMAP_ERROR;
    } else if (reg_dump & 0x20) {
        return INT_STATUS__ODR_50HZ_ERROR;
    }

    // if none of the above, idk what the error is (should not happen)
    return INT_STATUS__UNKNOWN;
}

void log_gyro_int_status_err(BMI270_INT_STATUS_ERR_t err) {
    switch (err) {
        case INT_STATUS__NOT_INIT:
            LOGE("INT_STATUS__NOT_INIT: ASIC not initialized");
            break;
        case INT_STATUS__INIT_OK:
            LOGI("INT_STATUS__INIT_OK: ASIC initialized");
            break;
        case INT_STATUS__INIT_ERR:
            LOGE("INT_STATUS__INIT_ERR: Initialization error");
            break;
        case INT_STATUS__DRV_ERR:
            LOGE("INT_STATUS__DRV_ERR: Invalid driver");
            break;
        case INT_STATUS__SNS_STOP:
            LOGE("INT_STATUS__SNS_STOP: Sensor stopped");
            break;
        case INT_STATUS__NVM_ERROR:
            LOGE("INT_STATUS__NVM_ERROR: Internal error while accessing NVM");
            break;
        case INT_STATUS__START_UP_ERROR:
            LOGE("INT_STATUS__START_UP_ERROR: Internal error while accessing NVM and initialization error");
            break;
        case INT_STATUS__COMPAT_ERROR:
            LOGE("INT_STATUS__COMPAT_ERROR: Compatibility error");
            break;
        case INT_STATUS__AXES_REMAP_ERROR:
            LOGE("INT_STATUS__AXES_REMAP_ERROR: Incorrect axes remapping, XYZ axes must be mapped to exclusively separate axes i.e. they cannot be mapped to same axis.");
            break;
        case INT_STATUS__ODR_50HZ_ERROR:
            LOGE("INT_STATUS__ODR_50HZ_ERROR: The minimum bandwidth conditions are not respected for the features which require 50 Hz data.");
            break;
        case INT_STATUS__UNKNOWN:
            LOGW("INT_STATUS__UNKNOWN: Unknown error (should not happen)");
            break;
    }
}