//
// Created by Isaak on 2/21/2024.
//

#ifndef BETTERFLIGHT_CLI_H
#define BETTERFLIGHT_CLI_H

#include "version.h"

typedef enum {
    CLI_CMD_NONE,
    CLI_CMD_HELP,
    CLI_CMD_STATUS,
    CLI_CMD_VERSION,
    CLI_CMD_CONNECT,
    CLI_CMD_SAVE,
} cli_cmd_t;

typedef struct {
    version_t version;
} cli_t;

void cli_process();
void cli_rx_callback();

#endif //BETTERFLIGHT_CLI_H
