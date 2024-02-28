/*

  This file is part of the CLI module.
  It contains the functionallity to handle the CLI commands.

  You should NOT ADD COMMANDS HERE. Use cli_cmd_callbacks.c instead.

 */

#include "cli.h"

#include <stdint.h>

#include "usbd_cdc_if.h"

#include "log.h"
#include "version.h"
#include "misc.h"
#include "cli_cmd_callbacks.h"
#include "main.h"

void cli_handle_cmd(cli_handle_t *cli_h);

void cli_process(void *arg) {
    if (arg == NULL) {
        LOGE("cli_process: arg is NULL! Not processing cli... unsafe!");
        return;
    }
    cli_handle_t *cli_h = (cli_handle_t *)arg;

    if (cli_h->new_data_flag) { // if new data data in buffer
        cli_h->new_data_flag = false;
        LOG("-> %s", cli_h->cli_rx_buffer);
        cli_handle_cmd(cli_h);
    }
}

void cli_init(cli_handle_t *cli_h) {
    LOGD("CLI Init...");

    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOGD("CLI is on version %s", version_string);

    //init the cmd_list array to NULL
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        cli_h->cmd_list[i].cmd_str = NULL;
    }

    cli_h->new_data_flag = false;

    add_commands(cli_h);
}

void cli_handle_cmd(cli_handle_t *cli_h) {
    // find the command index
    uint8_t *cmd_str = cli_h->cli_rx_buffer;
    int8_t cmd_index = -1;

    for (int8_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (cli_h->cmd_list[i].cmd_str != NULL) {
            if (strcmp_ign(cli_h->cmd_list[i].cmd_str, cmd_str) == 0) {
                cmd_index = i;
                break;
            }
        }
    }

    if (cmd_index == -1) {
        LOGE("The command you entered does not exist: %s, try again", cmd_str);
        return;
    }

    if (cli_h->cmd_list[cmd_index].cmd_callback != NULL) {
        cli_h->cmd_list[cmd_index].cmd_callback(cli_h);
    } else {
        LOGE("The command you entered has no callback, it is most likely not implemented yet: %s", cmd_str);
    }
}


void cli_add_cmd(cli_handle_t *cli_h, uint8_t *cmd_str, callback_t cmd_callback) {

    //check if cmd_str is already an existing command
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (strcmp_ign(cmd_str, (uint8_t *)cli_h->cmd_list[i].cmd_str) == 0) {
            LOGE((uint8_t *)"Command already exists, not adding... %s", cmd_str);
            return;
        }
    }

    //find an empty slot in the command list
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (cli_h->cmd_list[i].cmd_str == NULL) {
            cli_h->cmd_list[i].cmd_str = cmd_str;
            cli_h->cmd_list[i].cmd_callback = cmd_callback;

            if (cmd_callback == NULL) {
                LOGW((uint8_t *)"Command added without callback: %s", cmd_str);
                return;
            }

            LOGD((uint8_t *)"Command added: %s", cmd_str);
            return;
        }
    }

    LOGE((uint8_t *)"No empty slot found for command %s, Increase MAX_CMD_COUNT", cmd_str);
}

// cli_rx_callback gets called when cli_rx_buffer gets updated with new data
void cli_rx_callback(cli_handle_t* cli_h) {
    cli_h->new_data_flag = true;
    // don't copy data here... will break receive function.
    //TODO: Add a true callback...
}