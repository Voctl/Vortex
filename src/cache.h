#ifndef CACHE_H
#define CACHE_H

#include "vortex.h"

int  cache_save_leaderboard(const vortex_leaderboard *lb);
int  cache_load_leaderboard(vortex_leaderboard *lb);
void cache_purge(void);

#endif
