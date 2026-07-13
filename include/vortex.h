#ifndef VORTEX_H
#define VORTEX_H

#define VORTEX_VERSION "0.1.0"
#define VORTEX_CONFIG_DIR "/.config/vortex"
#define VORTEX_CACHE_FILE "/cache"
#define VORTEX_CONFIG_FILE "/config"
#define VORTEX_CACHE_TTL 3600

#define API_BASE_URL "https://githubcommunity.az/api/v1"

typedef enum {
    CMD_DASHBOARD,
    CMD_LEADERBOARD,
    CMD_LEADERBOARD_ALL,
    CMD_STATUS,
    CMD_NEWS,
    CMD_HELP,
    CMD_VERSION
} vortex_command;

typedef struct {
    char api_key[256];
    char api_base_url[512];
    int refresh_interval;
} vortex_config;

typedef struct {
    char username[128];
    char name[128];
    int rank;
    int total_contributors;
    int commits_today;
    int commits_week;
    int pull_requests;
    int issues;
    int reviews;
} vortex_user_status;

typedef struct {
    char username[128];
    char name[128];
    int rank;
    int commits;
    int pull_requests;
    int issues;
    int reviews;
} vortex_contributor;

typedef struct {
    vortex_contributor *entries;
    int count;
} vortex_leaderboard;

typedef struct {
    char title[256];
    char date[64];
} vortex_news_item;

typedef struct {
    vortex_news_item *items;
    int count;
} vortex_news;

#endif
