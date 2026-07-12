#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static int config_dir_exists(void)
{
    char path[512];
    const char *home = getenv("HOME");
    if (!home) return 0;
    snprintf(path, sizeof(path), "%s%s", home, VORTEX_CONFIG_DIR);
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

static int create_config_dir(void)
{
    char path[512];
    const char *home = getenv("HOME");
    if (!home) return -1;
    snprintf(path, sizeof(path), "%s%s", home, VORTEX_CONFIG_DIR);
    return mkdir(path, 0755);
}

static void config_path(char *buf, size_t len)
{
    const char *home = getenv("HOME");
    snprintf(buf, len, "%s%s%s", home ? home : "/tmp", VORTEX_CONFIG_DIR, VORTEX_CONFIG_FILE);
}

void config_set_defaults(vortex_config *cfg)
{
    strncpy(cfg->api_key, "", sizeof(cfg->api_key) - 1);
    strncpy(cfg->api_base_url, API_BASE_URL, sizeof(cfg->api_base_url) - 1);
    cfg->refresh_interval = 300;
}

int config_load(vortex_config *cfg)
{
    config_set_defaults(cfg);

    if (!config_dir_exists())
        return -1;

    char path[512];
    config_path(path, sizeof(path));

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n') continue;

        char *key = strtok(p, "= \t\n");
        char *val = strtok(NULL, "\n");
        if (!key || !val) continue;
        while (*val == ' ' || *val == '\t') val++;

        if (strcmp(key, "api_key") == 0)
            strncpy(cfg->api_key, val, sizeof(cfg->api_key) - 1);
        else if (strcmp(key, "api_base_url") == 0)
            strncpy(cfg->api_base_url, val, sizeof(cfg->api_base_url) - 1);
        else if (strcmp(key, "refresh_interval") == 0)
            cfg->refresh_interval = atoi(val);
    }
    fclose(f);
    return 0;
}

int config_save(const vortex_config *cfg)
{
    if (!config_dir_exists())
        create_config_dir();

    char path[512];
    config_path(path, sizeof(path));

    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "# Vortex configuration\n");
    fprintf(f, "api_key = %s\n", cfg->api_key);
    fprintf(f, "api_base_url = %s\n", cfg->api_base_url);
    fprintf(f, "refresh_interval = %d\n", cfg->refresh_interval);
    fclose(f);
    return 0;
}
