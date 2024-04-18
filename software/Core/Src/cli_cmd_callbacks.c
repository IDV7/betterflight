/*

 This file is part of the CLI module.
 It contains the callback functions that are called when a command is received.

 You have to be here and NOT IN CLI.C to add new commands

 To add a command:
    1. Add a new callback function
    2. Add the callback function and cli command string to the add_commands function (below cb's)
*/

#include "cli_cmd_callbacks.h"

#include <stdint.h>
#include <stdio.h>

#include "cli.h"
#include "log.h"
#include "version.h"
#include "misc.h"
#include "main.h"
#include "dshot.h"
#include "mymain.h"

void cli_cb_status(cli_handle_t *cli_h) {
    LOGI("Status: OK");
}

void cli_cb_help(cli_handle_t *cli_h) {
    LOG("[RSP] Available commands:");
    for (uint8_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (cli_h->cmd_list[i].cmd_str != NULL) {
            LOG("%s", cli_h->cmd_list[i].cmd_str);
        }
    }
}

void cli_cb_dfu(cli_handle_t *cli_h) {
    LOGI("Rebooting into DFU mode...");
    reboot_into_dfu();
}

void cli_cb_reboot(cli_handle_t *cli_h) {
    LOGI("Rebooting...");
    HAL_Delay(100); //wait for msg to be sent
    NVIC_SystemReset();
}

void cli_cb_version(cli_handle_t *cli_h) {
    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOG("[RSP] %s", version_string);
}

void cli_cb_connect(cli_handle_t *cli_h) {
    LOG((uint8_t *) "[RSP] Confirm Connection");
    cli_h->cli_connected_flag = true;
}

void cli_cb_disconnect(cli_handle_t *cli_h) {
    LOG((uint8_t *) "[RSP] Confirm Disconnection");
    cli_h->cli_connected_flag = false;
}

void cli_cb_clidemo(cli_handle_t *cli_h) {
    LOGD("This is a Debug message");
    LOGI("This is an Info message");
    LOGW("This is a Warning message");
    LOGE("This is an Error message");
    LOG("This is a normal message (should not be used...)");
    LOG("[RSP] This is a response message to a cli command");
}

callback_t cli_cb_none(cli_handle_t *cli_h) {
    LOGI("Command not implemented yet");
}

void cli_cb_dshotdemo(cli_handle_t *cli_h) {
    LOG((uint8_t *) "[RSP] Dshot demo starting");
    dshot_send(&m2_h, (uint16_t *) 1000);
}

void cli_cb_set_motors(cli_handle_t *cli_h) {
    uint16_t motor_values[5];
    if (cli_h->last_args_count == 1) {
        for (uint8_t i = 0; i < 4; i++) {
            motor_values[i] = (char_to_uint16((char*)cli_h->last_args[0]));
        }
    } else if (cli_h->last_args_count == 4) {
        for (uint8_t i = 0; i < 4; ++i) {
            motor_values[i] = (char_to_uint16((char*)cli_h->last_args[i]));
            delay(1);
        }
    } else {
        LOGE("Invalid number of arguments, 1 to set all motors to the same value, 4 to set each motor individually!");
        return;
    }

    LOG("[RSP] Set Motor valuese to: m1: %d m2: %d m3: %d m4: %d", motor_values[0], motor_values[1], motor_values[2], motor_values[3]);

    dshot_set_speed(&m1_h, motor_values[0]);
    dshot_set_speed(&m2_h, motor_values[1]);
    dshot_set_speed(&m3_h, motor_values[2]);
    dshot_set_speed(&m4_h, motor_values[3]);

}

void cli_cb_testargs(cli_handle_t *cli_h) {
    LOG("[RSP] Test Args");
    for (uint8_t i = 0; i < cli_h->last_args_count; i++) {
        LOGI("Arg %d: %s", i, cli_h->last_args[i]);
    }
}



void add_commands(cli_handle_t *cli_h) {
    cli_add_cmd(cli_h, (uint8_t *)"help", (callback_t) cli_cb_help);
    cli_add_cmd(cli_h, (uint8_t *)"status", (callback_t) cli_cb_status);
    cli_add_cmd(cli_h, (uint8_t *)"version", (callback_t) cli_cb_version);
    cli_add_cmd(cli_h, (uint8_t *)"connect", (callback_t) cli_cb_connect);
    cli_add_cmd(cli_h, (uint8_t *)"save", NULL);
    cli_add_cmd(cli_h, (uint8_t *)"dfu", (callback_t) cli_cb_dfu);
    cli_add_cmd(cli_h, (uint8_t *)"reboot", (callback_t) cli_cb_reboot);
    cli_add_cmd(cli_h, (uint8_t *)"disconnect", (callback_t) cli_cb_disconnect);
    cli_add_cmd(cli_h, (uint8_t *)"demo", (callback_t) cli_cb_clidemo);
    cli_add_cmd(cli_h, (uint8_t *)"dshot", (callback_t) cli_cb_dshotdemo);
    cli_add_cmd(cli_h, (uint8_t *)"testargs", (callback_t) cli_cb_testargs);
    cli_add_cmd(cli_h, (uint8_t *)"setmotors", (callback_t) cli_cb_set_motors);
}
