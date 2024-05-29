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
#define CLI_RX_BUFF_SIZE 64
#define CLI_TX_BUFF_SIZE 512

typedef struct {
    char cmd_str[MAX_CMD_STR_LEN];
    callback_t cmd_callback;
}cli_cmd_t;

typedef struct {
    // flags/options
    bool halt_until_connected_opt; // set to true if you want to wait for a "connect" command before starting the main application (useful for debugging)
    bool disable_log_opt; // set to true if you want to disable logging from the CLI, AND SAVE CPU TIME
    bool cli_connected_flag;
    bool new_data_flag;


    version_t version;

    // rx/tx
    bool enable_tx_buffering_opt; // set to false if you want to send data immediately (no buffering, you can overload the serial port and data will be lost if you send too much)
    bool tx_buff_empty_flag;
    uint16_t tx_buffer_len;
    char cli_rx_buffer[CLI_RX_BUFF_SIZE];
    char cli_tx_buffer[CLI_TX_BUFF_SIZE];


    // cmd related
    cli_cmd_t cmd_list[MAX_CMD_COUNT];
    char* last_arg_tokens[MAX_ARGS_COUNT];
    uint8_t last_cmd_index;
    uint8_t last_args_count;
} cli_handle_t;

void cli_init(cli_handle_t *CLI_h);
void cli_process(void *arg);

void cli_rx_callback(cli_handle_t* CLI_h);
void cli_add_cmd(cli_handle_t* CLI_h, char *cmd_str, callback_t cmd_callback);


#endif //BETTERFLIGHT_CLI_H
