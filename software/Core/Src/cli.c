//
// Created by Isaak on 2/21/2024.
//

#include "cli.h"

#include <stdint.h>

#include "usbd_cdc_if.h"

#include "log.h"
#include "version.h"

uint8_t cli_rx_buffer[64];
bool new_data_flag = false;

void cli_handle_cmd(uint8_t *cmd_str);
cli_cmd_t str_to_cmd(uint8_t *msg);

void cli_init(cli_t cli) {
    LOGD("CLI Init...");
    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOGD("CLI is on version %s", version_string);


}

void cli_process() {
    if (new_data_flag) {
        new_data_flag = false;
        cli_handle_cmd(cli_rx_buffer);
    }
}



void cli_handle_cmd(uint8_t * cmd_str) {
    cli_cmd_t cmd = str_to_cmd(cmd_str);

    if (cmd != CLI_CMD_NONE) {
        LOG("-> %s", cmd_str);
    }


    switch (cmd) {
        case CLI_CMD_HELP:
            LOGI("Help command received");
            break;
        case CLI_CMD_STATUS:
            LOGI("Status command received");
            break;
        case CLI_CMD_VERSION:
        {
            uint8_t version_string[10];
            VERSION_TO_STRING(CLI_VERSION, version_string);
            LOG("[RSP] %s", version_string);
            break;
        }
        case CLI_CMD_CONNECT:
            LOGI("Connect command received");
            break;
        case CLI_CMD_SAVE:
            LOGI("Save command received");
            break;
        case CLI_CMD_DFU:
            LOGW("Reseting into DFU mode...");
            HAL_Delay(100);
            reboot_into_dfu();
            break;
        case CLI_CMD_NONE:
            LOGI("Unknown command received");
            break;
    }
}

cli_cmd_t str_to_cmd(uint8_t *msg) {
    if (strcmp_ign(msg, "help") == 0) {
        return CLI_CMD_HELP;
    } else if (strcmp_ign(msg, "status") == 0) {
        return CLI_CMD_STATUS;
    } else if (strcmp_ign(msg, "version") == 0) {
        return CLI_CMD_VERSION;
    } else if (strcmp_ign(msg, "connect") == 0) {
        return CLI_CMD_CONNECT;
    }else if (strcmp_ign(msg, "save") == 0) {
        return CLI_CMD_SAVE;
    } else if (strcmp_ign(msg, "dfu") == 0) {
        return CLI_CMD_DFU;
    } else {
        return CLI_CMD_NONE;
    }
}

// cli_rx_callback gets called when cli_rx_buffer gets updated with new data
void cli_rx_callback() {
    new_data_flag = true;
    // don't copy data here... will break receive function.
    //TODO: Add a true callback...
}