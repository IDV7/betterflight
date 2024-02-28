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


void cli_cb_help(cli_handle_t *cli_h) {
    LOGI("Available commands:");
    for (uint8_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (cli_h->cmd_list[i].cmd_str != NULL) {
            LOGI("%s", cli_h->cmd_list[i].cmd_str);
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


callback_t cli_cb_none(cli_handle_t *cli_h) {
    LOGI("Command not implemented yet");
}

void add_commands(cli_handle_t *cli_h) {
    cli_add_cmd(cli_h, (uint8_t *)"help", (callback_t) cli_cb_help);
    cli_add_cmd(cli_h, (uint8_t *)"status", NULL);
    cli_add_cmd(cli_h, (uint8_t *)"version", (callback_t) cli_cb_version);
    cli_add_cmd(cli_h, (uint8_t *)"connect", NULL);
    cli_add_cmd(cli_h, (uint8_t *)"save", NULL);
    cli_add_cmd(cli_h, (uint8_t *)"dfu", (callback_t) cli_cb_dfu);
    cli_add_cmd(cli_h, (uint8_t *)"reboot", (callback_t) cli_cb_none);
}