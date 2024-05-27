//
// Created by Isaak on 2/21/2024.
//

#ifndef BETTERFLIGHT_CLI_H
#define BETTERFLIGHT_CLI_H

#include "version.h"
#include "misc.h"

#define MAX_CMD_COUNT 64
#define MAX_ARGS_COUNT 16

typedef struct {
    uint8_t *cmd_str;
    callback_t cmd_callback;
}cli_cmd_t;

typedef struct {
    cli_cmd_t cmd_list[MAX_CMD_COUNT];
    version_t version;
    uint8_t cli_rx_buffer[64];
    uint8_t cli_tx_buffer[64];
    bool new_data_flag;
    bool cli_connected_flag;
    bool halt_until_connected_flag;
    uint8_t last_cmd_index;
    uint8_t *last_args[MAX_ARGS_COUNT];
    uint8_t last_args_count;
} cli_handle_t;

void cli_init(cli_handle_t *cli_h);
void cli_process(void *arg);

void cli_rx_callback(cli_handle_t* cli_h);
void cli_add_cmd(cli_handle_t* cli_h, uint8_t *cmd_str, callback_t cmd_callback);


#endif //BETTERFLIGHT_CLI_H