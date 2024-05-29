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

//motors
dshot_handle_t m1_h; // TIM1 CH2
dshot_handle_t m2_h; // TIM1 CH1
dshot_handle_t m3_h; // TIM8 CH4
dshot_handle_t m4_h; // TIM8 CH3
motors_handle_t motors_h;


//pid
drone_pids_t pids_h;
//mixer
mixer_handle_t motor_mixer_h;


uint64_t led_toggle_last_ms = 0;
uint64_t cli_process_last_ms = 0;
uint64_t pid_controller_test_ms = 0;
uint64_t crsf_last_ms = 0;
uint64_t motors_process_last_ms = 0;
uint64_t imu_process_last_ms = 0;
uint64_t flight_ctrl_cycle_last_ms = 0;

crsf_handle_t crsf_h;


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
    pid_init(&pids_h);
//
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
        none_blocking_delay(500, &imu_process_last_ms, (callback_t) imu_process, &imu_h);
    }
}