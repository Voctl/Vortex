#include "ui.h"
#include <stdio.h>
#include <string.h>

void ui_print_banner(void)
{
    printf("\n");
    printf("  " HABAMAX_CYAN HABAMAX_BOLD "\u250C\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510" HABAMAX_RESET "\n");
    printf("  " HABAMAX_CYAN HABAMAX_BOLD "\u2502" HABAMAX_RESET "            " HABAMAX_ORANGE "V O R T E X" HABAMAX_RESET "            " HABAMAX_CYAN HABAMAX_BOLD "\u2502" HABAMAX_RESET "\n");
    printf("  " HABAMAX_CYAN HABAMAX_BOLD "\u2502" HABAMAX_RESET "  Terminal Gateway for Open Source  " HABAMAX_CYAN HABAMAX_BOLD "\u2502" HABAMAX_RESET "\n");
    printf("  " HABAMAX_CYAN HABAMAX_BOLD "\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518" HABAMAX_RESET "\n");
    printf("\n");
}

static const char *rank_color(int rank)
{
    if (rank == 1) return HABAMAX_ORANGE;
    if (rank <= 3) return HABAMAX_YELLOW;
    if (rank <= 10) return HABAMAX_GREEN;
    return HABAMAX_WHITE;
}

static const char *commit_color(int commits)
{
    if (commits >= 50) return HABAMAX_ORANGE;
    if (commits >= 20) return HABAMAX_YELLOW;
    if (commits >= 5)  return HABAMAX_GREEN;
    return HABAMAX_GREY;
}

void ui_print_leaderboard(const vortex_leaderboard *lb)
{
    printf("  " HABAMAX_BOLD HABAMAX_BLUE "LEADERBOARD  Azerbaijan GitHub Community" HABAMAX_RESET "\n");
    printf("  " HABAMAX_GREY "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500" HABAMAX_RESET "\n");
    printf("  " HABAMAX_GREY "  #  Contributor                 C   PR   I   R" HABAMAX_RESET "\n");
    printf("  " HABAMAX_GREY "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500" HABAMAX_RESET "\n");

    int max_show = lb->count < 10 ? lb->count : 10;
    for (int i = 0; i < max_show; i++) {
        vortex_contributor *c = &lb->entries[i];
        const char *medal = "";
        if (c->rank == 1) medal = " \xF0\x9F\xA5\x87";
        else if (c->rank == 2) medal = " \xF0\x9F\xA5\x88";
        else if (c->rank == 3) medal = " \xF0\x9F\xA5\x89";

        printf("  %s#%-3d%s" HABAMAX_RESET "  %s%-22s" HABAMAX_RESET
               "  %s%3d" HABAMAX_RESET "  %s%2d" HABAMAX_RESET
               "  %s%2d" HABAMAX_RESET "  %s%2d" HABAMAX_RESET "\n",
               rank_color(c->rank), c->rank, medal,
               HABAMAX_BOLD, c->username,
               commit_color(c->commits), c->commits,
               c->pull_requests > 0 ? HABAMAX_GREEN : HABAMAX_GREY, c->pull_requests,
               c->issues > 0 ? HABAMAX_YELLOW : HABAMAX_GREY, c->issues,
               c->reviews > 0 ? HABAMAX_CYAN : HABAMAX_GREY, c->reviews);
    }
    printf("\n");
}

void ui_print_status(const vortex_user_status *st)
{
    printf("  " HABAMAX_BOLD HABAMAX_BLUE "STATUS" HABAMAX_RESET "\n");
    printf("  " HABAMAX_GREY "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500" HABAMAX_RESET "\n");

    printf("  " HABAMAX_CYAN "User:" HABAMAX_RESET "     %s%s" HABAMAX_RESET "\n", HABAMAX_BOLD, st->username);
    if (st->name[0])
        printf("  " HABAMAX_CYAN "Name:" HABAMAX_RESET "     %s\n", st->name);
    printf("  " HABAMAX_CYAN "Rank:" HABAMAX_RESET "     %s#%d" HABAMAX_RESET " / %d\n",
           rank_color(st->rank), st->rank, st->total_contributors);
    if (st->commits_week > 0) {
        printf("  " HABAMAX_CYAN "Commits:" HABAMAX_RESET "  %s%d" HABAMAX_RESET " (%s)\n",
               commit_color(st->commits_week), st->commits_week,
               st->commits_today < 0 ? "all-time" : "month");
        printf("  " HABAMAX_CYAN "PRs:" HABAMAX_RESET "     %s%d" HABAMAX_RESET "\n",
               st->pull_requests > 0 ? HABAMAX_GREEN : HABAMAX_GREY, st->pull_requests);
        printf("  " HABAMAX_CYAN "Issues:" HABAMAX_RESET "  %s%d" HABAMAX_RESET "\n",
               st->issues > 0 ? HABAMAX_YELLOW : HABAMAX_GREY, st->issues);
        printf("  " HABAMAX_CYAN "Reviews:" HABAMAX_RESET "  %s%d" HABAMAX_RESET "\n",
               st->reviews > 0 ? HABAMAX_CYAN : HABAMAX_GREY, st->reviews);
    } else {
        printf("  " HABAMAX_GREY "  No data found for this user" HABAMAX_RESET "\n");
    }
    printf("\n");
}

void ui_print_news(const vortex_news *news)
{
    printf("  " HABAMAX_BOLD HABAMAX_BLUE "COMMUNITY NEWS" HABAMAX_RESET "\n");
    printf("  " HABAMAX_GREY "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500" HABAMAX_RESET "\n");

    for (int i = 0; i < news->count; i++) {
        vortex_news_item *item = &news->items[i];
        printf("  " HABAMAX_ORANGE "\u2022" HABAMAX_RESET " %s%s" HABAMAX_RESET "\n", HABAMAX_BOLD, item->title);
        printf("    " HABAMAX_GREY "%s" HABAMAX_RESET "\n", item->date);
        printf("\n");
    }
}

void ui_print_error(const char *msg)
{
    printf("  " HABAMAX_RED "Error:" HABAMAX_RESET " %s\n", msg);
}
