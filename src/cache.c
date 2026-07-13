#include "cache.h"
#include "vortex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static void cache_dir(char *buf, size_t len)
{
    const char *home = getenv("HOME");
    snprintf(buf, len, "%s%s/cache", home ? home : "/tmp", VORTEX_CONFIG_DIR);
}

static int ensure_cache_dir(void)
{
    char path[512];
    cache_dir(path, sizeof(path));
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        return 0;
    const char *home = getenv("HOME");
    if (!home) return -1;
    snprintf(path, sizeof(path), "%s%s", home, VORTEX_CONFIG_DIR);
    if (stat(path, &st) != 0)
        mkdir(path, 0755);
    char cdir[512];
    cache_dir(cdir, sizeof(cdir));
    return mkdir(cdir, 0755);
}

static void file_path(const char *key, const char *suffix, char *buf, size_t len)
{
    char dir[512];
    cache_dir(dir, sizeof(dir));
    snprintf(buf, len, "%s/%s%s", dir, key, suffix ? suffix : "");
}

int cache_get(const char *key, char **data)
{
    char path[512], tspath[512];
    file_path(key, NULL, path, sizeof(path));
    file_path(key, ".ts", tspath, sizeof(tspath));

    FILE *f = fopen(tspath, "r");
    if (!f) return -1;
    long cached_ts;
    if (fscanf(f, "%ld", &cached_ts) != 1) { fclose(f); return -1; }
    fclose(f);

    time_t now = time(NULL);
    if (now - cached_ts > VORTEX_CACHE_TTL) {
        remove(path);
        remove(tspath);
        return -1;
    }

    f = fopen(path, "r");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    *data = malloc(len + 1);
    if (!*data) { fclose(f); return -1; }
    size_t n = fread(*data, 1, len, f);
    fclose(f);
    (*data)[n] = '\0';
    return 0;
}

int cache_set(const char *key, const char *data)
{
    if (ensure_cache_dir() != 0) return -1;

    char path[512], tspath[512];
    file_path(key, NULL, path, sizeof(path));
    file_path(key, ".ts", tspath, sizeof(tspath));

    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fwrite(data, 1, strlen(data), f);
    fclose(f);

    f = fopen(tspath, "w");
    if (!f) return -1;
    time_t now = time(NULL);
    fprintf(f, "%ld", (long)now);
    fclose(f);
    return 0;
}

void cache_purge(void)
{
    char dir[512];
    cache_dir(dir, sizeof(dir));
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "rm -rf '%s' 2>/dev/null", dir);
    system(cmd);
}
