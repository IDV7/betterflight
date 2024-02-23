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

void cli_handle_msg(uint8_t * msg);

void cli_init(cli_t cli) {
    LOGD("CLI Init...");
    uint8_t version_string[10];
    VERSION_TO_STRING(CLI_VERSION, version_string);
    LOGD("CLI is on version %s", version_string);


}

void cli_process() {
    if (new_data_flag) {
        new_data_flag = false;
        //cli_handle_msg(cli_rx_buffer);
        LOGD("New message received -> %s", cli_rx_buffer);
    }
}



// cli_rx_callback gets called when cli_rx_buffer gets updated with new data
void cli_rx_callback() {
    new_data_flag = true;
    // don't copy data here... will break receive function.
    //TODO: Add a true callback...
}