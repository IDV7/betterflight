

#ifndef BETTERFLIGHT_IMU_H
#define BETTERFLIGHT_IMU_H

#endif //BETTERFLIGHT_IMU_H

#include "spi_soft.h"
#include "bmi270.h"


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
} IMU_err_t;

typedef struct {
    SPI_handle_t spi_h;
    uint8_t sensor_list[2];
    struct bmi2_dev bmi;
    struct bmi2_sens_data sensor_data;
    float acc_x, acc_y, acc_z;
    float gyr_x, gyr_y, gyr_z;
    struct bmi2_sens_config config;
    IMU_err_t last_err; //this is the last error that occured in the imu_process function. instead of returning the error code it gets saved here to be handled
} IMU_handle_t;

// for implementation of imu.c
IMU_err_t imu_init(IMU_handle_t *imu_h);
void imu_process(IMU_handle_t *imu_h);
void log_imu_err(IMU_err_t err);
void log_gyr_acc_data(IMU_handle_t *imu_h);

// bmi270 api needs access to these functions
int8_t imu_spi_read_reg(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr); // function pointers with api from bmi270
int8_t imu_spi_write_reg(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void imu_delay_us(uint32_t period, void *intf_ptr);
