#include "mymain.h"

#include <stdio.h>

#include "main.h"
#include "misc.h"
#include "log.h"
#include "cli.h"
#include "dshot.h"
#include "motors.h"
#include "imu.h"
#include "crsf.h"
#include "pid_controller.h"


#define LOG_LEVEL LOG_LEVEL_DEBUG

// for precise timing related debugging (measured with logic analyzer)
#define S2_HIGH (GPIOB->BSRR = GPIO_PIN_4)
#define S2_LOW (GPIOB->BSRR = (GPIO_PIN_4 << 16))

IMU_handle_t imu_h;
cli_handle_t cli_h;


//general
flight_axis_t channel_data;
flight_measurements_t imu_data;


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
uint64_t pid_controller_test_ms = 0;
uint64_t crsf_last_ms = 0;
uint64_t motors_process_last_ms = 0;
uint64_t imu_process_last_ms = 0;
uint64_t flight_ctrl_cycle_last_ms = 0;

crsf_handle_t crsf_h;

int16_t received_data[16];

bool is_armed_flag = false;


static void flight_ctrl_cycle(void);

void myinit(void) {
    cli_h.halt_until_connected_opt = true; //set to false if you don't want to wait for a connection
    cli_h.enable_tx_buffering_opt = false; //false for init (procces has to be runned to put out buffered data)
    cli_h.cli_disable_log_opt = false; // completely disables logging (returns from log functions immediately to save CPU time)



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

   // cli_h.enable_tx_buffering_opt = false; //enable tx buffering NOTE: data will be buffered from now on, and ONLY be sent when cli_process is called!!
}



void mymain(void) {
    while (1) { //todo has to be replaced by a scheduler
        none_blocking_delay(1000, &led_toggle_last_ms, (callback_t) LED_toggle, NULL);
        none_blocking_delay(25, &cli_process_last_ms, (callback_t) cli_process, &cli_h);
        none_blocking_delay(1, &motors_process_last_ms, (callback_t) motors_process, &motors_h);
        //none_blocking_delay(500, &imu_process_last_ms, (callback_t) imu_process, &imu_h);
        flight_ctrl_cycle();
    }
}

static void flight_ctrl_cycle(void) {
    //crsf + imu =>pid's=>motor mixe
    int16_t temp;

    // update elrs channels
    crsf_process(&crsf_h, received_data);

    //LOGD("Data: yaw=%d pitch=%d thr=%d roll=%d, arm: %d", received_data[0], received_data[1], received_data[2], received_data[3],received_data[4]);
    delay(10);
    channel_data.yaw = map( received_data[0], 172, 1811, -500, 500);
    channel_data.pitch = map(received_data[1], 172, 1811, -500, 500);
    channel_data.thr= map(received_data[2], 172, 1811, 50, 2012);
    channel_data.roll = map(received_data[3], 172, 1811, -500, 500);
    temp = map(received_data[4],172,1811,988,2012);

    if(((int16_t) received_data[4]) > 1500){
        is_armed_flag = true;
    }
    else{
        is_armed_flag = false;
    }
    //LOGD("Channel data: roll=%d pitch=%d yaw=%d throttle=%d, arm %d", channel_data.roll, channel_data.pitch, channel_data.yaw, channel_data.thr, temp);


    if(is_armed_flag == true){


    // update imu data
    if (imu_h.last_err == IMU_OK) {
        imu_process(&imu_h);
    }
    imu_data.roll = imu_h.gyr_x;
    imu_data.pitch = imu_h.gyr_y;
    imu_data.yaw = imu_h.gyr_z;

    //calculate setpoints
    s_points.roll_set_point.sp = set_point_calculation(&pids_h.setp,channel_data.roll,(float) 0.2,(float)0.2);
    s_points.pitch_set_point.sp = set_point_calculation(&pids_h.setp,channel_data.pitch,(float)0.2,(float)0.2);
    s_points.yaw_set_point.sp = set_point_calculation(&pids_h.setp,channel_data.yaw,(float)0.2,(float)0.2);

    //LOGD("setpoints: roll=%d pitch=%d yaw=%d", s_points.roll_set_point.sp, s_points.pitch_set_point.sp, s_points.yaw_set_point.sp);
        //delay(10);
    // update pid controllers
    //LOGD("imu data: roll=%d pitch=%d yaw=%d", imu_data.roll, imu_data.pitch, imu_data.yaw);
       // delay(10);
    set_pids(&pids_h, &imu_data, &s_points, &pid_vals);
    //LOGD("pid vals: roll=%d pitch=%d yaw=%d throttle %d", pid_vals.roll_pid, pid_vals.pitch_pid, pid_vals.yaw_pid, channel_data.thr);
      //  delay(10);
    // update motor mixer

   // delay(1);
    motor_mixer_h.input.yaw = pid_vals.yaw_pid;
    motor_mixer_h.input.roll = pid_vals.roll_pid;
    motor_mixer_h.input.pitch = pid_vals.pitch_pid;
    motor_mixer_h.input.throttle = channel_data.thr;
    mixing(&motor_mixer_h, &motor_output);
    LOGD("motor output: motor1=%d motor2=%d motor3=%d motor4=%d", motor_output.motor1, motor_output.motor2, motor_output.motor3, motor_output.motor4);
    motor_set_throttle(&motors_h,1, motor_output.motor1);
    motor_set_throttle(&motors_h,2, motor_output.motor2);
    motor_set_throttle(&motors_h,3, motor_output.motor3);
    motor_set_throttle(&motors_h,4, motor_output.motor4);

}
    else{
        //motor_stop(&motors_h,1);
        motors_stop(&motors_h);
        LOGD("Motors stopped");
    }
    //delay(1);
   //delay(50);
}