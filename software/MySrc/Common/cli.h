//
// Created by Isaak on 2/21/2024.
//

#ifndef BETTERFLIGHT_CLI_H
#define BETTERFLIGHT_CLI_H

#include "version.h"
#include "misc.h"

#define MAX_CMD_COUNT 64
#define MAX_ARGS_COUNT 16
#define MAX_CMD_STR_LEN 32

typedef struct {
    char cmd_str[MAX_CMD_STR_LEN];
    callback_t cmd_callback;
}cli_cmd_t;

typedef struct {
    cli_cmd_t cmd_list[MAX_CMD_COUNT];
    version_t version;
    char cli_rx_buffer[64];
    char cli_tx_buffer[64];
    bool new_data_flag;
    bool cli_connected_flag;
    bool halt_until_connected_flag;
    uint8_t last_cmd_index;
    char* last_arg_tokens[MAX_ARGS_COUNT];
    uint8_t last_args_count;
} cli_handle_t;

void cli_init(cli_handle_t *CLI_h);
void cli_process(void *arg);

void cli_rx_callback(cli_handle_t* CLI_h);
void cli_add_cmd(cli_handle_t* CLI_h, char *cmd_str, callback_t cmd_callback);


#endif //BETTERFLIGHT_CLI_H
