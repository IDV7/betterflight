

#ifndef BETTERFLIGHT_IMU_H
#define BETTERFLIGHT_IMU_H

#endif //BETTERFLIGHT_IMU_H

#include "bmi270.h"

#include "spi_soft.h"
#include "common_structs.h"

typedef enum {
    IMU_OK = 0,
    IMU_ERR_BMI_INIT = 1,
    IMU_ERR_ACC_GYR_CONFIG = 2,
    IMU_ERR_SENSOR_ENABLE = 3,
    IMU_ERR_GET_SENSOR_CONFIG = 4,
    IMU_ERR_GET_SENSOR_DATA = 5,
    IMU_WARN_GYRO_READ_NOT_READY = 6,
    IMU_WARN_ACC_READ_NOT_READY = 7,
    IMU_WARN_GYRO_AND_ACC_READ_NOT_READY = 8,
    IMU_ERR_POC = 9,
} IMU_err_t;

typedef struct {
    SPI_handle_t spi_h;
    uint8_t sensor_list[2];
    struct bmi2_dev bmi;
    struct bmi2_sens_data sensor_data;
    flight_axis_float_t acc;
    flight_axis_float_t gyr;
    struct bmi2_sens_config config;
    IMU_err_t last_err; //this is the last error that occured in the imu_process function. instead of returning the error code it gets saved here to be handled
} IMU_handle_t;

//inits imu chip via spi and sets up the imu handle
IMU_err_t imu_init(IMU_handle_t *imu_h);

// reads sensor data from imu and saves it in the imu handle
void imu_process(IMU_handle_t *imu_h);

// logs the error code
void log_imu_err(IMU_err_t err);

// logs the imu data
void log_imu_data(IMU_handle_t *imu_h);

// (DEPRECATED DON'T USE) reads out data regs
void imu_get_gyr_data(IMU_handle_t *imu_h);

// spi_soft_trx conversion to api requested api receive function
int8_t spi_trx_api_wrapper(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr); // function pointers with api from bmi270

// spi_soft_rx conversion to api requested api write function
int8_t spi_tx_api_wrapper(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

// wraps around delay_us for api use
void delay_us_api_wrapper(uint32_t period, void *intf_ptr);

// it doens't do anything its just to give the api something to call (when it wants to delay which it is not allowed to do anymore after init)
void imu_no_delay_wrapper(uint32_t period, void *intf_ptr);
