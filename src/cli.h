#ifndef CLI_H
#define CLI_H

#include "vortex.h"

typedef struct {
    vortex_command command;
    char username[128];
} vortex_args;

int cli_parse(int argc, char **argv, vortex_args *args);
void cli_print_help(void);

#endif
