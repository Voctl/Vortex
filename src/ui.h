#ifndef UI_H
#define UI_H

#include "vortex.h"

#define HABAMAX_RED     "\033[38;5;1m"
#define HABAMAX_GREEN   "\033[38;5;2m"
#define HABAMAX_YELLOW  "\033[38;5;3m"
#define HABAMAX_BLUE    "\033[38;5;4m"
#define HABAMAX_CYAN    "\033[38;5;6m"
#define HABAMAX_WHITE   "\033[38;5;7m"
#define HABAMAX_GREY    "\033[38;5;8m"
#define HABAMAX_ORANGE  "\033[38;5;9m"
#define HABAMAX_RESET   "\033[0m"
#define HABAMAX_BOLD    "\033[1m"

void ui_print_banner(void);
void ui_print_leaderboard(const vortex_leaderboard *lb);
void ui_print_status(const vortex_user_status *st);
void ui_print_news(const vortex_news *news);
void ui_print_error(const char *msg);

#endif
