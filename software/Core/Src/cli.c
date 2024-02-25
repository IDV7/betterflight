//
// Created by Isaak on 2/21/2024.
//

#include "cli.h"

#include <stdint.h>

#include "usbd_cdc_if.h"

#include "log.h"
#include "version.h"

#define CMD_COUNT (sizeof(cli_cmd_table) / sizeof(cli_cmd_table[0]))

typedef struct {
    const char* cmd_str;
    cli_cmd_t cmd_enum;
} cli_cmd_info_t;

const cli_cmd_info_t cli_cmd_table[] = {
        {"help",    CLI_CMD_HELP},
        {"status",  CLI_CMD_STATUS},
        {"version", CLI_CMD_VERSION},
        {"connect", CLI_CMD_CONNECT},
        {"dfu",     CLI_CMD_DFU},
        {"save",    CLI_CMD_SAVE},
        {"reboot",  CLI_CMD_REBOOT},
        {"dev1", CLI_CMD_DEV1},
        {"dev2", CLI_CMD_DEV2},
        {"dev3", CLI_CMD_DEV3}
};

uint8_t cli_rx_buffer[64];
bool new_data_flag = false;

void cli_handle_cmd(uint8_t *cmd_str);
cli_cmd_t str_to_cmd(const uint8_t *msg);

extern void dev1_callback(void);
extern void dev2_callback(void);
extern void dev3_callback(void);


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
        case CLI_CMD_REBOOT:
            LOGW("Rebooting...");
            HAL_Delay(100);
            NVIC_SystemReset();
            break;
        case CLI_CMD_DEV1:
            dev1_callback();
            break;
        case CLI_CMD_DEV2:
            dev2_callback();
            break;
        case CLI_CMD_DEV3:
            dev3_callback();
            break;
        case CLI_CMD_NONE:
            LOGI("Unknown command received");
            break;
    }
}

cli_cmd_t str_to_cmd(const uint8_t *msg) {
    for (size_t i = 0; i < CMD_COUNT; i++) {
        if (strcmp_ign(msg, (uint8_t *)cli_cmd_table[i].cmd_str) == 0) {
            return cli_cmd_table[i].cmd_enum;
        }
    }
    return CLI_CMD_NONE;
}

// cli_rx_callback gets called when cli_rx_buffer gets updated with new data
void cli_rx_callback() {
    new_data_flag = true;
    // don't copy data here... will break receive function.
    //TODO: Add a true callback...
}