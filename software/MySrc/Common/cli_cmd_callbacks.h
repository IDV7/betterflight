
#ifndef BETTERFLIGHT_CLI_CMD_CALLBACKS_H
#define BETTERFLIGHT_CLI_CMD_CALLBACKS_H

#include "cli.h"

// this function contains all the individual command inits (should be called by cli_init)
void add_commands(cli_handle_t *cli_h);

#endif //BETTERFLIGHT_CLI_CMD_CALLBACKS_H
