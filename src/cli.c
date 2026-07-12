#include "cli.h"
#include <stdio.h>
#include <string.h>

int cli_parse(int argc, char **argv, vortex_args *args)
{
    memset(args, 0, sizeof(*args));
    args->command = CMD_DASHBOARD;

    if (argc < 2)
        return 0;

    if (strcmp(argv[1], "leaderboard") == 0) {
        args->command = CMD_LEADERBOARD;
        if (argc > 2 && strcmp(argv[2], "--all") == 0)
            args->command = CMD_LEADERBOARD_ALL;
    } else if (strcmp(argv[1], "status") == 0) {
        args->command = CMD_STATUS;
        if (argc > 2)
            strncpy(args->username, argv[2], sizeof(args->username) - 1);
    } else if (strcmp(argv[1], "news") == 0) {
        args->command = CMD_NEWS;
    } else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0) {
        args->command = CMD_HELP;
    } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        args->command = CMD_VERSION;
    } else {
        args->command = CMD_HELP;
    }

    return 0;
}

void cli_print_help(void)
{
    printf("Vortex v%s - Terminal Gateway for Open Source\n\n", VORTEX_VERSION);
    printf("Usage:\n");
    printf("  vortex                    Dashboard (auto-refresh every 5h)\n");
    printf("  vortex leaderboard        Monthly leaderboard (current month)\n");
    printf("  vortex leaderboard --all  All-time leaderboard\n");
    printf("  vortex status [username]  Personal ranking and progress\n");
    printf("  vortex news               Latest community announcements\n");
    printf("  vortex help               Show this help message\n");
    printf("  vortex --version          Show version info\n");
}
