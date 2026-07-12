#include "vortex.h"
#include "config.h"
#include "cache.h"
#include "api.h"
#include "cli.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define REFRESH_INTERVAL 18000  /* 5 saat */

static void clear_screen(void)
{
    printf("\033[2J\033[H");
}

int main(int argc, char **argv)
{
    vortex_config cfg;
    vortex_args args;
    int ret = 0;

    config_load(&cfg);
    cli_parse(argc, argv, &args);

    switch (args.command) {
    case CMD_DASHBOARD: {
        for (;;) {
            vortex_leaderboard lb = {0};
            vortex_user_status st = {0};
            vortex_news news = {0};

            const char *user = getenv("USER");
            snprintf(args.username, sizeof(args.username), "%s", user ? user : "unknown");

            api_fetch_monthly_leaderboard(&cfg, &lb);
            api_fetch_status(&cfg, args.username, &st);
            api_fetch_news(&cfg, &news);

            clear_screen();
            ui_print_banner();

            time_t now = time(NULL);
            struct tm *tm = localtime(&now);
            char date_str[64];
            strftime(date_str, sizeof(date_str), "%B %Y", tm);
            printf("  " HABAMAX_GREY "Monthly Leaderboard \xE2\x80\x94 %s" HABAMAX_RESET "\n\n", date_str);

            ui_print_leaderboard(&lb);
            ui_print_status(&st);
            ui_print_news(&news);

            printf("  " HABAMAX_GREY "Auto-refresh every 5h. Press Ctrl+C to exit." HABAMAX_RESET "\n");

            free(lb.entries);
            free(news.items);

            sleep(REFRESH_INTERVAL);
        }
        break;
    }
    case CMD_LEADERBOARD: {
        vortex_leaderboard lb = {0};
        api_fetch_monthly_leaderboard(&cfg, &lb);
        cache_save_leaderboard(&lb);
        ui_print_banner();

        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        char date_str[64];
        strftime(date_str, sizeof(date_str), "%B %Y", tm);
        printf("  " HABAMAX_GREY "Monthly Leaderboard \xE2\x80\x94 %s" HABAMAX_RESET "\n\n", date_str);

        ui_print_leaderboard(&lb);
        free(lb.entries);
        break;
    }
    case CMD_LEADERBOARD_ALL: {
        vortex_leaderboard lb = {0};
        api_fetch_leaderboard(&cfg, &lb);
        cache_save_leaderboard(&lb);
        ui_print_banner();

        printf("  " HABAMAX_GREY "All-Time Leaderboard \xE2\x80\x94 Azerbaijan GitHub Community" HABAMAX_RESET "\n\n");

        ui_print_leaderboard(&lb);
        free(lb.entries);
        break;
    }
    case CMD_STATUS: {
        if (args.username[0] == '\0') {
            const char *user = getenv("USER");
            snprintf(args.username, sizeof(args.username), "%s", user ? user : "unknown");
        }
        vortex_user_status st = {0};
        api_fetch_status(&cfg, args.username, &st);
        ui_print_banner();
        ui_print_status(&st);
        break;
    }
    case CMD_NEWS: {
        vortex_news news = {0};
        api_fetch_news(&cfg, &news);
        ui_print_banner();
        ui_print_news(&news);
        free(news.items);
        break;
    }
    case CMD_VERSION:
        printf("Vortex v%s\n", VORTEX_VERSION);
        printf("https://github.com/devuan/vortex\n");
        break;
    case CMD_HELP:
    default:
        ui_print_banner();
        cli_print_help();
        break;
    }

    return ret;
}
