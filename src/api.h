#ifndef API_H
#define API_H

#include "vortex.h"

int api_fetch_leaderboard(const vortex_config *cfg, vortex_leaderboard *lb);
int api_fetch_monthly_leaderboard(const vortex_config *cfg, vortex_leaderboard *lb);
int api_fetch_status(const vortex_config *cfg, const char *username, vortex_user_status *st);
int api_fetch_news(const vortex_config *cfg, vortex_news *news);

#endif
