/*
 The BMI270 api from Bosch is used to interface with the BMI270 IMU sensor.
 This code is based on the example code provided by Bosch.
 -> https://github.com/boschsensortec/BMI270_SensorAPI/blob/master/bmi270_examples/accel_gyro/accel_gyro.c
*/



#include "imu.h"
#include "bmi270.h" //has bmi2.h included => has bmi2_defs.h included
#include <math.h>

#include "log.h"
#include "spi_soft.h"


#define GRAVITY_EARTH  (9.80665f)

#define ACCEL          UINT8_C(0x00)
#define GYRO           UINT8_C(0x01)

/*

 bmi2_dev  //Structure to define BMI2 sensor configurations BEGIN

 important ones

 bmi2_intf => interface type
 uint8_t dummy_byte  => switching from i2c to spi
 const uint8_t *config_file_ptr;     // Pointer to the configuration data buffer address

 bmi2_delay_fptr_t delay_us;      //Delay function pointer


 bmi2_dev  Structure to define BMI2 sensor configurations   END

 */

// Global SPI handler because the BMI270's SPI interface doesn't work with this handler

static int8_t set_accel_gyro_config(struct bmi2_dev *bmi);
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

static int8_t set_accel_gyro_config(struct bmi2_dev *bmi);
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);
void bmi2_error_codes_print_result(int8_t rslt);


// Initializes the BMI270 using the Bosch BMI270 API
imu_err_t imu_init(IMU_handle_t *imu_h) { //todo return error code

    // spi init
    imu_h->spi_h.sck.port = GPIOA;
    imu_h->spi_h.sck.pin = GPIO_PIN_5;

    imu_h->spi_h.miso.port = GPIOA;
    imu_h->spi_h.miso.pin = GPIO_PIN_6;

    imu_h->spi_h.mosi.port = GPIOA;
    imu_h->spi_h.mosi.pin = GPIO_PIN_7;

    imu_h->spi_h.cs.port = GPIOB;
    imu_h->spi_h.cs.pin = GPIO_PIN_2;

    SPI_soft_init(&imu_h->spi_h);

    // bmi270 init
    int8_t rslt;

    imu_h->limit = 100; // limit to print accel data
    imu_h->sensor_list[0] = BMI2_ACCEL;
    imu_h->sensor_list[1] = BMI2_GYRO;

    struct bmi2_sens_data sensor_data = { { 0 } };
    imu_h->sensor_data = sensor_data;

    imu_h->indx = 0;
    imu_h->acc_x = 0; imu_h->acc_y = 0; imu_h->acc_z = 0;
    imu_h->gyr_x = 0; imu_h->gyr_y = 0; imu_h->gyr_z = 0;

    imu_h->bmi.intf = BMI2_SPI_INTF;
    imu_h->bmi.read = (bmi2_read_fptr_t)imu_spi_read_reg;
    imu_h->bmi.write = (bmi2_write_fptr_t)imu_spi_write_reg;
    imu_h->bmi.delay_us = (bmi2_delay_fptr_t)delay_us;

    LOGI("bmi270 init");
    rslt = bmi270_init(&imu_h->bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt != BMI2_OK) return IMU_ERR_BMI_INIT;

    LOGI("bmi270 set accel gyro config");
    rslt = set_accel_gyro_config(&imu_h->bmi);

    if (rslt != BMI2_OK) return IMU_ERR_ACC_GYR_CONFIG;

    rslt = bmi2_sensor_enable(imu_h->sensor_list, 2, &imu_h->bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt != BMI2_OK) return IMU_ERR_SENSOR_ENABLE;

    imu_h->config.type = BMI2_ACCEL;
    rslt = bmi2_get_sensor_config(&imu_h->config, 1, &imu_h->bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt != BMI2_OK) return IMU_ERR_GET_SENSOR_CONFIG;

    LOGI("bmi270 succesfully initialized");
    return IMU_OK;
}



// reads out the accelerometer and gyroscope data from the BMI270 and stores it in the IMU_handle_t struct
void imu_process(IMU_handle_t *imu_h) {
    int8_t rslt;

    struct bmi2_sens_data sensor_data = { { 0 } };

    rslt = bmi2_get_sensor_data(&imu_h->sensor_data, &imu_h->bmi);
    bmi2_error_codes_print_result(rslt);


    if (rslt != BMI2_OK) {
        imu_h->last_err = IMU_ERR_GET_SENSOR_DATA; // save error code for later handling
        return;
    }

    if (imu_h->sensor_data.status & BMI2_DRDY_ACC) {
        imu_h->acc_x = lsb_to_mps2(imu_h->sensor_data.sens_data[0].x, 2, 16);
        imu_h->acc_y = lsb_to_mps2(imu_h->sensor_data.sens_data[0].y, 2, 16);
        imu_h->acc_z = lsb_to_mps2(imu_h->sensor_data.sens_data[0].z, 2, 16);

    }
    else {
        imu_h->last_err = IMU_WARN_ACC_READ_NOT_READY;
    }

    if (imu_h->sensor_data.status & BMI2_DRDY_GYR) {
        imu_h->gyr_x = lsb_to_dps(imu_h->sensor_data.sens_data[1].x, 2000, 16);
        imu_h->gyr_y = lsb_to_dps(imu_h->sensor_data.sens_data[1].y, 2000, 16);
        imu_h->gyr_z = lsb_to_dps(imu_h->sensor_data.sens_data[1].z, 2000, 16);
    }
    else {
        if (imu_h->last_err == IMU_WARN_ACC_READ_NOT_READY)  // if both acc and gyro are not ready
            imu_h->last_err = IMU_WARN_GYRO_AND_ACC_READ_NOT_READY;
        else // if only gyro is not ready
            imu_h->last_err = IMU_WARN_GYRO_READ_NOT_READY;
    }
}

// alias for SPI_soft_read (to feed in spi_h handler struct)
int8_t imu_spi_read_reg(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t tx_buffer[] = {(reg_addr | 0x80), 0x00, 0x00}; // | 0x80 to set the read bit
    uint8_t rx_buffer[] = {0x00, 0x00, 0x00};

    SPI_soft_trx(intf_ptr, &reg_addr, reg_data, 3);
    return 0;
}


// alias for SPI_soft_write (to feed in spi_h handler struct)
int8_t imu_spi_write_reg(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t tx_buffer[] = {reg_addr, *reg_data};
    uint8_t rx_buffer[] = {0x00, 0x00};

    SPI_soft_trx(intf_ptr, tx_buffer, rx_buffer, 2);
    return 0;
}


void imu_delay_us(uint32_t period, void *intf_ptr) {
    delay(1);
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi2_set_sensor_config(config, 2, bmi);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

void bmi2_error_codes_print_result(int8_t rslt)
{
    switch (rslt)
    {
        case BMI2_OK:

            /* Do nothing */
            break;

        case BMI2_W_FIFO_EMPTY:
            LOGW("Warning [%d] : FIFO empty\r\n", rslt);
            break;
        case BMI2_W_PARTIAL_READ:
            LOGW("Warning [%d] : FIFO partial read\r\n", rslt);
            break;
        case BMI2_E_NULL_PTR:
            LOGE("Error [%d] : Null pointer error. It occurs when the user tries to assign value (not address) to a pointer," " which has been initialized to NULL.\r\n",rslt);
            break;

        case BMI2_E_COM_FAIL:
            LOGE("Error [%d] : Communication failure error. It occurs due to read/write operation failure and also due " "to power failure during communication\r\n", rslt);
                        break;

        case BMI2_E_DEV_NOT_FOUND:
            LOGE("Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read\r\n",rslt);
            break;

        case BMI2_E_INVALID_SENSOR:
            LOGE("Error [%d] : Invalid sensor error. It occurs when there is a mismatch in the requested feature with the " "available one\r\n", rslt);
            break;

        case BMI2_E_SELF_TEST_FAIL:
            LOGE("Error [%d] : Self-test failed error. It occurs when the validation of accel self-test data is " "not satisfied\r\n",rslt);
            break;

        case BMI2_E_INVALID_INT_PIN:
            LOGE("Error [%d] : Invalid interrupt pin error. It occurs when the user tries to configure interrupt pins " "apart from INT1 and INT2\r\n",rslt);
            break;

        case BMI2_E_OUT_OF_RANGE:
            LOGE("Error [%d] : Out of range error. It occurs when the data exceeds from filtered or unfiltered data from " "fifo and also when the range exceeds the maximum range for accel and gyro while performing FOC\r\n",rslt);
            break;

        case BMI2_E_ACC_INVALID_CFG:
            LOGE("Error [%d] : Invalid Accel configuration error. It occurs when there is an error in accel configuration" " register which could be one among range, BW or filter performance in reg address 0x40\r\n",rslt);
            break;

        case BMI2_E_GYRO_INVALID_CFG:
            LOGE("Error [%d] : Invalid Gyro configuration error. It occurs when there is a error in gyro configuration" "register which could be one among range, BW or filter performance in reg address 0x42\r\n",rslt);
            break;

        case BMI2_E_ACC_GYR_INVALID_CFG:
            LOGE("Error [%d] : Invalid Accel-Gyro configuration error. It occurs when there is a error in accel and gyro" " configuration registers which could be one among range, BW or filter performance in reg address 0x40 " "and 0x42\r\n",rslt);
            break;

        case BMI2_E_CONFIG_LOAD:
            LOGE("Error [%d] : Configuration load error. It occurs when failure observed while loading the configuration " "into the sensor\r\n",rslt);
            break;

        case BMI2_E_INVALID_PAGE:
            LOGE("Error [%d] : Invalid page error. It occurs due to failure in writing the correct feature configuration " "from selected page\r\n",rslt);
            break;

        case BMI2_E_SET_APS_FAIL:
            LOGE("Error [%d] : APS failure error. It occurs due to failure in write of advance power mode configuration " "register\r\n",rslt);
            break;

        case BMI2_E_AUX_INVALID_CFG:
            LOGE("Error [%d] : Invalid AUX configuration error. It occurs when the auxiliary interface settings are not " "enabled properly\r\n",rslt);
            break;

        case BMI2_E_AUX_BUSY:
            LOGE("Error [%d] : AUX busy error. It occurs when the auxiliary interface buses are engaged while configuring" " the AUX\r\n",rslt);
            break;

        case BMI2_E_REMAP_ERROR:
            LOGE("Error [%d] : Remap error. It occurs due to failure in assigning the remap axes data for all the axes " "after change in axis position\r\n",rslt);
            break;

        case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
            LOGE("Error [%d] : Gyro user gain update fail error. It occurs when the reading of user gain update status " "fails\r\n",rslt);
            break;

        case BMI2_E_SELF_TEST_NOT_DONE:
            LOGE("Error [%d] : Self-test not done error. It occurs when the self-test process is ongoing or not " "completed\r\n",rslt);
            break;

        case BMI2_E_INVALID_INPUT:
            LOGE("Error [%d] : Invalid input error. It occurs when the sensor input validity fails\r\n", rslt);
            break;

        case BMI2_E_INVALID_STATUS:
            LOGE("Error [%d] : Invalid status error. It occurs when the feature/sensor validity fails\r\n", rslt);
            break;

        case BMI2_E_CRT_ERROR:
            LOGE("Error [%d] : CRT error. It occurs when the CRT test has failed\r\n", rslt);
            break;

        case BMI2_E_ST_ALREADY_RUNNING:
            LOGE("Error [%d] : Self-test already running error. It occurs when the self-test is already running and " "another has been initiated\r\n",rslt);
            break;

        case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
            LOGE("Error [%d] : CRT ready for download fail abort error. It occurs when download in CRT fails due to wrong " "address location\r\n",rslt);
            break;

        case BMI2_E_DL_ERROR:
            LOGE("Error [%d] : Download error. It occurs when write length exceeds that of the maximum burst length\r\n",rslt);
            break;

        case BMI2_E_PRECON_ERROR:
            LOGE("Error [%d] : Pre-conditional error. It occurs when precondition to start the feature was not " "completed\r\n",rslt);
            break;

        case BMI2_E_ABORT_ERROR:
            LOGE("Error [%d] : Abort error. It occurs when the device was shaken during CRT test\r\n", rslt);
            break;

        case BMI2_E_WRITE_CYCLE_ONGOING:
            LOGE("Error [%d] : Write cycle ongoing error. It occurs when the write cycle is already running and another " "has been initiated\r\n", rslt);
            break;

        case BMI2_E_ST_NOT_RUNING:
            LOGE("Error [%d] : Self-test is not running error. It occurs when self-test running is disabled while it's " "running\r\n", rslt);
            break;

        case BMI2_E_DATA_RDY_INT_FAILED:
            LOGE("Error [%d] : Data ready interrupt error. It occurs when the sample count exceeds the FOC sample limit " "and data ready status is not updated\r\n", rslt);
            break;

        case BMI2_E_INVALID_FOC_POSITION:
            LOGE( "Error [%d] : Invalid FOC position error. It occurs when average FOC data is obtained for the wrong" " axes\r\n", rslt);
            break;

        default:
            LOGE("Error [%d] : Unknown error code\r\n", rslt);
            break;
    }
}
