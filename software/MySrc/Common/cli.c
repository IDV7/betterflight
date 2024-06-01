

#include "cli.h"

#include <sys/unistd.h>
#include <sys/errno.h>

#include "usbd_cdc_if.h"

#include "log.h"
#include "version.h"
#include "misc.h"
#include "cli_cmd_callbacks.h"
#include "mymain.h"


void cli_handle_cmd(cli_handle_t *cli_h);

// cli_process manages send and receive data.
void cli_process(void *arg) {
    if (arg == NULL) {
        LOGE("cli_process: arg is NULL! Not processing cli... unsafe!");
        return;
    }
    cli_handle_t *CLI_h = (cli_handle_t *)arg;

    // read data
    if (CLI_h->new_data_flag) { // if new data data in buffer
        CLI_h->new_data_flag = false;
        LOG("-> %s", CLI_h->cli_rx_buffer);
        cli_handle_cmd(CLI_h); // handle the data
    }

    //write data (from buffer)
    if (CLI_h->enable_tx_buffering_opt && !CLI_h->tx_buff_empty_flag) { //if buffering is enabled and buffer is not empty
        CDC_Transmit_FS((uint8_t*) CLI_h->cli_tx_buffer, CLI_h->tx_buffer_len); // send the entire buffer
        CLI_h->tx_buff_empty_flag = true;
        CLI_h->tx_buffer_len = 0;
    }
}
// cli_init initializes the CLI module, sets up the command list and waits for a connection when halt_until_connected_opt is set to true
void cli_init(cli_handle_t *CLI_h) {
    LOGD("CLI Init...");

    CLI_h->cli_connected_flag = false;
    CLI_h->new_data_flag = false;
    CLI_h->tx_buff_empty_flag = true;
    CLI_h->tx_buffer_len = 0;

    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOGD("CLI is on version %s", version_string);

    //empty the command list
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        memset(CLI_h->cmd_list->cmd_str, 0, sizeof(CLI_h->cmd_list->cmd_str));
        CLI_h->cmd_list[i].cmd_callback = NULL;
    }


    add_commands(CLI_h);

    //wait for connection
    if (CLI_h->cli_connected_flag == false && CLI_h->halt_until_connected_opt == true) {
        LOGD("Waiting for connection...");
        while (CLI_h->cli_connected_flag == false) {
            LED_toggle();
            delay(100);
            if (CLI_h->new_data_flag == true) {
                CLI_h->new_data_flag = false;

                // check for "connect" cmd
                if (strcmp_ign(CLI_h->cli_rx_buffer, "connect") == 0) {
                    CLI_h->cli_connected_flag = true;
                    LOGD("Connection confirmed");
                }
                //check for "dfu" cmd
                if (strcmp_ign(CLI_h->cli_rx_buffer, "dfu") == 0) {
                    reboot_into_dfu();
                }
            }
        }
    }
}

// cli_handle_cmd handles the incoming data, it splits the data into a command and arguments and calls the appropriate callback function
void cli_handle_cmd(cli_handle_t *CLI_h) {
    // take first word as command
    char *cmd_str = strtok((char *)CLI_h->cli_rx_buffer, " ");

    //clean arg array
    for (int i = 0; i < MAX_ARGS_COUNT; i++) {
        CLI_h->last_arg_tokens[i] = "";
    }

    // array to hold the arguments
    uint8_t arg_count = 0;

    // get the rest of the arguments
    while (arg_count < MAX_ARGS_COUNT) {
        CLI_h->last_arg_tokens[arg_count] = strtok(NULL, " ");
        if (CLI_h->last_arg_tokens[arg_count] == NULL) {
            break;
        }
        clean_str(CLI_h->last_arg_tokens[arg_count]); // removes any \n or \r
        arg_count++;
    }
    CLI_h->last_args_count = arg_count; //save last count so cmd callbacks know how many args there are

    int8_t cmd_index = -1;

    // find the command in the command
    for (int8_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (strcmp_ign(CLI_h->cmd_list[i].cmd_str, cmd_str) == 0) {
            cmd_index = i; // found the command, save its index
            break;
        }
    }

    // tell the user if the command was not found
    if (cmd_index == -1) {
        LOGE("The command you entered does not exist: %s, try again", cmd_str);
        return;
    }

    // call the command callback
    if (CLI_h->cmd_list[cmd_index].cmd_callback != NULL) {
        CLI_h->cmd_list[cmd_index].cmd_callback(CLI_h);
    } else { // (command exists but no callback was set)
        LOGE("The command you entered (\"%s\") has no callback, it is most likely not implemented yet", cmd_str);
    }
}

// this function lets a dev easily add a new command
void cli_add_cmd(cli_handle_t *CLI_h, char *cmd_str, callback_t cmd_callback) {
    //check if cmd_str is already an existing command
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (strcmp_ign(cmd_str, CLI_h->cmd_list[i].cmd_str) == 0) {
            LOGE("Command already exists, not adding... %s", cmd_str);
            return;
        }
    }

    //find an empty slot in the command list
    for (size_t i = 0; i < MAX_CMD_COUNT; i++) {
        if (CLI_h->cmd_list[i].cmd_str[0] == '\0') { //if slot is empty
            memcpy(CLI_h->cmd_list[i].cmd_str, cmd_str, MAX_CMD_STR_LEN);
            CLI_h->cmd_list[i].cmd_callback = cmd_callback;

            if (cmd_callback == NULL) {
                LOGW("Command added without callback: %s", cmd_str);
                return;
            }

            LOGD("Command added: %s", cmd_str);
            return;
        }
    }

    LOGE("No empty slot found for command %s, Increase MAX_CMD_COUNT", cmd_str);
}

// cli_rx_callback gets called by the HAL when cli_rx_buffer gets updated with new data
void cli_rx_callback(cli_handle_t* CLI_h) {
    CLI_h->new_data_flag = true;
    // don't copy data here... will break receive function.
    //TODO: Add a true callback...
}

#define RETRIES 5
#define RETRY_DELAY_US 100 //us


// this overrides the _write function printf uses to output data, its implemented here to send data via CDC (so vcom port serial)
int _write(int file, char *ptr, int len) {
    if (cli_h.disable_log_opt) return 0; //return 0 "success but no bytes were sent"

    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        if (cli_h.enable_tx_buffering_opt && cli_h.cli_connected_flag) { // if buffering is enabled and a user is connected via cli buffer data
            for (int i = 0; i < len; i++) {
                if (cli_h.tx_buffer_len > CLI_TX_BUFF_SIZE - 1) {
                    break;
                }
                cli_h.cli_tx_buffer[cli_h.tx_buffer_len] = ptr[i];
                cli_h.tx_buffer_len++;
            }
            //add \n
            cli_h.tx_buffer_len++;
            cli_h.cli_tx_buffer[cli_h.tx_buffer_len] = '\n';

            cli_h.tx_buff_empty_flag = false;
        }
        else if (cli_h.cli_connected_flag) { // if no buffering is enabled, send data directly
            uint8_t rslt = CDC_Transmit_FS((uint8_t*)ptr, len);
            if (rslt == USBD_BUSY){ // in case HAL tells us its already sending something try again a few times with a delay
                for (int i = 0; i < RETRIES; i++){
                    delay_us(RETRY_DELAY_US);
                    rslt = CDC_Transmit_FS((uint8_t*)ptr, len);
                    if (rslt == USBD_OK){
                        break;
                    }
                }
            }
        }
        return len;
    }
        errno = EBADF;
        return -1;
}