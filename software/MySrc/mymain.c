/*
 * Running on STM32F722ret6
 * using expresslrs (RF-transmitter and RF-receiver) and dshot protocol (ESC protocol for controlling the motors)
 * BMI270 IMU is used to get the gyro data of the drone, accelerometer data can be implemented in the future to increase the stability of the drone
 *
 * The code in this file combines all the files into one coherent program and a flying drone
 */

#include "mymain.h"

#include "main.h"
#include "math.h"
#include "log.h"
#include "imu.h"
#include "crsf.h"
#include "pid_controller.h"


#define LOG_LEVEL LOG_LEVEL_DEBUG
#define ever ;;


TIM_HandleTypeDef htim_flight_cycle;

IMU_handle_t imu_h;
cli_handle_t cli_h;

bool crc8_confirmed_flag = false;
//general
flight_axis_t channel_data;
flight_axis_int16_t imu_data;


//motors
dshot_handle_t m1_h; // TIM1 CH2
dshot_handle_t m2_h; // TIM1 CH1
dshot_handle_t m3_h; // TIM8 CH4
dshot_handle_t m4_h; // TIM8 CH3
motors_handle_t motors_h;


//pid
flight_pids_t pids_h;
pids_val_t pid_vals;
//mixer
mixer_handle_t motor_mixer_h;
//setpoint
set_points_t s_points;


motor_output_t motor_output;

uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;
uint64_t log_stats_last_ms = 0;


crsf_handle_t crsf_h;

uint32_t received_data[16];
bool is_armed_flag = false;


static void flight_ctrl_cycle(void);
void tim_flight_cycle_init(void);
void log_stats(void);

void myinit(void) {
    cli_h.halt_until_connected_opt = false; //set to false if you don't want to wait for a connection
    cli_h.enable_tx_buffering_opt = false; //false for init (procces has to be runned to put out buffered data)
    cli_h.disable_log_opt = false; // completely disables logging (returns from log functions immediately to save CPU time)



    log_init(LOG_LEVEL, true);
    LED_on();

    setup_delay_us_tim();

    cli_init(&cli_h); //will block(halt) if halt_until_connected_opt is true (until a connection is made)

    LOGI("Starting Initialization...");


    // ----- all initialization code goes here ----- //
    //init imu
    log_imu_err(imu_init(&imu_h));

    //elrs init
    crsf_init(&crsf_h, &huart2);

    //pid init
    init_mixer_percentages(&motor_mixer_h);
    pid_init(&pids_h);

    // motors init
    dshot_init(&m1_h, &htim1, &hdma_tim1_ch2, TIM_CHANNEL_2);
    dshot_init(&m2_h, &htim1, &hdma_tim1_ch1, TIM_CHANNEL_1);
    dshot_init(&m3_h, &htim8, &hdma_tim8_ch4_trig_com, TIM_CHANNEL_4);
    dshot_init(&m4_h, &htim8, &hdma_tim8_ch3, TIM_CHANNEL_3);
    motors_init(&motors_h, &m1_h, &m2_h, &m3_h, &m4_h);
    //     ----- end initialization code ----- //


    LOGI("Finished Initialization");

    LED_blink_pattern(20, 2, 50, 50);
    LED_off();

    cli_h.enable_tx_buffering_opt = false; //enable tx buffering NOTE: data will be buffered from now on, and ONLY be sent when cli_process is called!!
    cli_h.disable_log_opt = false;
}

static void S2_toggle(void) {
    static uint8_t state = 0;
    if (state) {
        S2_HIGH;
    } else {
        S2_LOW;
    }
    state = !state;
}

#define FLIGHT_CYCLE_FREQUENCY 625 //us
uint32_t flight_cycle_time = 0;

void log_stats(void){
    if (cli_h.cli_connected_flag) {
        //LOGD("HAL_TIM_CNT: %d", __HAL_TIM_GET_COUNTER(&htim2));
        LOGI("Flight cycle time (target is 625us): %dus", flight_cycle_time);

       // LOGD("gyr: roll=%.2f pitch=%.2f yaw=%.2f (from imu_h)", imu_h.gyr.roll, imu_h.gyr.pitch, imu_h.gyr.yaw);
//        LOGD("gyr: roll=%d pitch=%d yaw=%d", imu_data.roll, imu_data.pitch, imu_data.yaw);
//        LOGD("Armed: %d", is_armed_flag);

    }
}


void mymain(void) {
    tim_flight_cycle_init();
    for(ever) {
        __HAL_TIM_SET_COUNTER(&htim_flight_cycle, 0); //reset timer counter

        // ----- times sensitive flight control code goes here ----- //


        flight_ctrl_cycle();
        motors_process(&motors_h);


        // ^^^^^ end of time sensitive flight control code ^^^^^ //

        while (__HAL_TIM_GET_COUNTER(&htim_flight_cycle) < FLIGHT_CYCLE_FREQUENCY) { // do other stuff while waiting
            if (__HAL_TIM_GET_COUNTER(&htim_flight_cycle) < (FLIGHT_CYCLE_FREQUENCY - (FLIGHT_CYCLE_FREQUENCY / 3))) { // if we are not near the last 20% of wait_time_us, we can do other stuff. else don't take risk of missing next flight cycle

                // ----- all non-critical code goes here ----- //
                none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
                none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
//                none_blocking_delay(1000, &log_stats_last_ms, (callback_t) log_stats, NULL);
                // ^^^^^ all non-critical code goes here ^^^^^ //

            }
        }
        flight_cycle_time = __HAL_TIM_GET_COUNTER(&htim_flight_cycle); //measure the time a cycle took
    }
}

static void flight_ctrl_cycle(void) {

    // update elrs channels
    crsf_process(&crsf_h, received_data, &crc8_confirmed_flag);

    if(crc8_confirmed_flag){
        int16_t ch5 = map(received_data[4],172,1811,988,2012);


        if(ch5 > 1700)
            is_armed_flag = true;
        else
            is_armed_flag = false;



        if(is_armed_flag == true) {

            channel_data.roll = map(received_data[0], 172, 1811, -500, 500);
            channel_data.pitch = -(map(received_data[1], 172, 1811, -500, 500));
            channel_data.thr= map(received_data[2], 172, 1811, 200, 1700);
            channel_data.yaw = map( received_data[3], 172, 1811, -500, 500);

            // update imu data
            if (imu_h.last_err == IMU_OK) {
                imu_process(&imu_h);
                imu_data.roll = (int16_t) (imu_h.gyr.roll);
                imu_data.pitch = (int16_t) (imu_h.gyr.pitch);
                imu_data.yaw = (int16_t) (-imu_h.gyr.yaw); //invert yaw => different coordinate system imu vs sticks
            }

            else {
                imu_data.roll = 0;
                imu_data.pitch = 0;
                imu_data.yaw = 0;
                LOGE("IMU error: %d, was unable to read", imu_h.last_err);
            }

            //calculate setpoints
            s_points.roll_set_point.sp = set_point_calculation(&pids_h.setp, channel_data.roll, (float) 0.1, (float) 0.2);
            s_points.pitch_set_point.sp = set_point_calculation(&pids_h.setp, channel_data.pitch, (float) 0.1, (float) 0.2);
            s_points.yaw_set_point.sp = set_point_calculation(&pids_h.setp, channel_data.yaw, (float) 0.1, (float) 0.2);

             // update pid controllers
            set_pids(&pids_h, &imu_data, &s_points, &pid_vals);


            // update motor mixer
            motor_mixer_h.input.yaw = pid_vals.yaw_pid;
            motor_mixer_h.input.roll = pid_vals.roll_pid;
            motor_mixer_h.input.pitch = pid_vals.pitch_pid;
            motor_mixer_h.input.throttle = channel_data.thr;

            mixing(&motor_mixer_h, &motor_output);


            motor_set_throttle(&motors_h, 1, motor_output.motor1);
            motor_set_throttle(&motors_h, 2, motor_output.motor2);
            motor_set_throttle(&motors_h, 3, motor_output.motor3);
            motor_set_throttle(&motors_h, 4, motor_output.motor4);

        } else{
            motors_stop(&motors_h);
        }
        crc8_confirmed_flag = false;
    }
}


void tim_flight_cycle_init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    htim_flight_cycle.Instance = TIM3;
    htim_flight_cycle.Init.Prescaler = 95; // 1 MHz (every 1 us)
    htim_flight_cycle.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_flight_cycle.Init.Period = 0xFFFFFFFF; // Max period
    htim_flight_cycle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim_flight_cycle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim_flight_cycle);
    HAL_TIM_Base_Start(&htim_flight_cycle);
}