/*

 This file is part of the CLI module.
 It contains the callback functions that are called when a command is received.
 And a function telling the CLI module which commands are available and which callback function to call when that command is received.

 You have to be here and NOT IN CLI.C to add new commands

 To add a command:
    1. Add a new callback function
    2. Add the callback function and cli command string to the add_commands function (below cb's)
*/

#include "cli_cmd_callbacks.h"

#include <stdio.h>

#include "cli.h"
#include "log.h"
#include "version.h"
#include "misc.h"
#include "main.h"
#include "mymain.h"
#include "motors.h"


void cli_cb_status(cli_handle_t *CLI_h) {
    LOGI("Status: OK");
}

// returns all available commands (not how to use them unfortunately)
void cli_cb_help(cli_handle_t *CLI_h) {
    LOG("[RSP] Available commands:");
    for (uint8_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (CLI_h->cmd_list[i].cmd_str[0] != '\0') {
            LOG("%s", CLI_h->cmd_list[i].cmd_str);
        }
    }
}

// reboots mcu into dfu mode (activates bootloader)
void cli_cb_dfu(cli_handle_t *CLI_h) {
    LOGI("Rebooting into DFU mode...");
    reboot_into_dfu();
}

// reboots mcu
void cli_cb_reboot(cli_handle_t *CLI_h) {
    LOGI("Rebooting...");
    HAL_Delay(100); //wait for msg to be sent
    NVIC_SystemReset();
}

// prints the version of the CLI module (not really used in the end..., it was once an idea)
void cli_cb_version(cli_handle_t *CLI_h) {
    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOG("[RSP] %s", version_string);
}

// confirms connection
void cli_cb_connect(cli_handle_t *CLI_h) {
    LOG("[RSP] Confirm Connection");
    CLI_h->cli_connected_flag = true;
}

// confirms disconnection (not really used)
void cli_cb_disconnect(cli_handle_t *CLI_h) {
    LOG("[RSP] Confirm Disconnection");
    CLI_h->cli_connected_flag = false;
}

// demo function to see how logging works
void cli_cb_clidemo(cli_handle_t *CLI_h) {
    LOGD("This is a Debug message");
    LOGI("This is an Info message");
    LOGW("This is a Warning message");
    LOGE("This is an Error message");
    LOG("This is a normal message (should not be used...)");
    LOG("[RSP] This is a response message to a cli command");
}

callback_t cli_cb_none(cli_handle_t *CLI_h) {
    LOGI("Command not implemented yet");
}

// --- MOTORS --- //

// sets a motor to a specific value
void cli_cb_set_motor(cli_handle_t *CLI_h) {
    LOG("[RSP] Setting motor %s to %s", (char*)CLI_h->last_arg_tokens[0], (char*)CLI_h->last_arg_tokens[1]);

    // check if the number of arguments is correct
    if (CLI_h->last_args_count != 2) {
        LOGE("Invalid number of arguments, 2 expected");
        return;
    }

    // check if the motor number is valid
    uint8_t motor_nr = char_to_uint16((char*)CLI_h->last_arg_tokens[0]);
    if (motor_nr < 1 || motor_nr > 4) {
        LOGE("Invalid motor number, 1 to 4 expected");
        return;
    }

    // convert the value to a uint16_t to be used by the motor driver
    uint16_t motor_value = char_to_uint16((char*)CLI_h->last_arg_tokens[1]);

    // set the motor value (will take effect on the next motor process cycle)
    motor_set_throttle(&motors_h, motor_nr, motor_value);
}

// control all motors with one command arg1 = m1, etc
void cli_cb_set_motors(cli_handle_t *CLI_h) {
    uint16_t motor_values[4];
    if (CLI_h->last_args_count == 1) {
        for (uint8_t i = 0; i < 4; i++) {
            motor_values[i] = (char_to_uint16((char*)CLI_h->last_arg_tokens[0]));
        }
    } else if (CLI_h->last_args_count == 4) {
        for (uint8_t i = 0; i < 4; ++i) {
            motor_values[i] = (char_to_uint16((char*)CLI_h->last_arg_tokens[i]));
            delay(1);
        }
    } else {
        LOGE("Invalid number of arguments, 1 to set all motors to the same value, 4 to set each motor individually!");
        return;
    }

    LOG("[RSP] Set Motor valuese to: m1: %d m2: %d m3: %d m4: %d", motor_values[0], motor_values[1], motor_values[2], motor_values[3]);
    motors_set_throttle_arr(&motors_h, motor_values);
}

// stops all motors
void cli_cb_stop_motors(cli_handle_t *CLI_h) {
    motors_stop(&motors_h);
    LOG("[RSP] Motors stopped");
}

void cli_cb_beep_motors(cli_handle_t *CLI_h) {
    motors_beep(&motors_h);
    LOG("[RSP] Motors beeped");
}

void cli_cb_set_motor_direction(cli_handle_t *CLI_h) {
    LOG("[RSP] Setting motor (%s) direction to %s", (char*)CLI_h->last_arg_tokens[0], (char*)CLI_h->last_arg_tokens[1]);

    if (CLI_h->last_args_count != 2) {
        LOGE("Invalid number of arguments, 2 expected");
        return;
    }

    uint8_t motor_nr = char_to_uint16((char*)CLI_h->last_arg_tokens[0]);
    if (motor_nr < 1 || motor_nr > 4) {
        LOGE("Invalid motor number, 1 to 4 expected");
        return;
    }

    bool motor_dir = char_to_uint16((char*)CLI_h->last_arg_tokens[1]);
    if (motor_dir != 0 && motor_dir != 1) {
        LOGE("Invalid motor direction, 0 or 1 expected");
        return;
    }

    motor_set_direction(&motors_h, motor_nr, motor_dir);
}

// --- EOF MOTORS --- //

void cli_cb_sendsc(cli_handle_t *CLI_h) {
    LOG("[RSP] Sending motor %d cmd %d" , char_to_uint16((char*)CLI_h->last_arg_tokens[0]), char_to_uint16((char*)CLI_h->last_arg_tokens[1]));
    dshot_send_special_command(motors_h.dshot_hs[char_to_uint16((char*)CLI_h->last_arg_tokens[0]) - 1], char_to_uint16((char*)CLI_h->last_arg_tokens[1]));
}

// add your callback with the corresponding command string here
void add_commands(cli_handle_t *CLI_h) {
    cli_add_cmd(CLI_h, "help", (callback_t) cli_cb_help);
    cli_add_cmd(CLI_h, "status", (callback_t) cli_cb_status);
    cli_add_cmd(CLI_h, "version", (callback_t) cli_cb_version);
    cli_add_cmd(CLI_h, "connect", (callback_t) cli_cb_connect);
    cli_add_cmd(CLI_h, "dfu", (callback_t) cli_cb_dfu);
    cli_add_cmd(CLI_h, "reboot", (callback_t) cli_cb_reboot);
    cli_add_cmd(CLI_h, "disconnect", (callback_t) cli_cb_disconnect);
    cli_add_cmd(CLI_h, "demo", (callback_t) cli_cb_clidemo);
    cli_add_cmd(CLI_h, "setmotor", (callback_t) cli_cb_set_motor);
    cli_add_cmd(CLI_h, "setmotors", (callback_t) cli_cb_set_motors);
    cli_add_cmd(CLI_h, "stopmotors", (callback_t) cli_cb_stop_motors);
    cli_add_cmd(CLI_h, "beepmotors", (callback_t) cli_cb_beep_motors);
    cli_add_cmd(CLI_h, "setmotordir", (callback_t) cli_cb_set_motor_direction);
    cli_add_cmd(CLI_h, "sendsc", (callback_t) cli_cb_sendsc);
}
