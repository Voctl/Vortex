#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static void cache_path(char *buf, size_t len)
{
    const char *home = getenv("HOME");
    snprintf(buf, len, "%s%s%s", home ? home : "/tmp", VORTEX_CONFIG_DIR, VORTEX_CACHE_FILE);
}

static int ensure_cache_dir(void)
{
    char path[512];
    const char *home = getenv("HOME");
    if (!home) return -1;
    snprintf(path, sizeof(path), "%s%s", home, VORTEX_CONFIG_DIR);
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        return 0;
    return mkdir(path, 0755);
}

int cache_save_leaderboard(const vortex_leaderboard *lb)
{
    if (ensure_cache_dir() != 0) return -1;

    char path[512];
    cache_path(path, sizeof(path));
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "leaderboard\n");
    fprintf(f, "count %d\n", lb->count);
    for (int i = 0; i < lb->count; i++) {
        fprintf(f, "entry %s %s %d %d %d %d %d\n",
                lb->entries[i].username,
                lb->entries[i].name,
                lb->entries[i].rank,
                lb->entries[i].commits,
                lb->entries[i].pull_requests,
                lb->entries[i].issues,
                lb->entries[i].reviews);
    }
    fclose(f);
    return 0;
}

int cache_load_leaderboard(vortex_leaderboard *lb)
{
    char path[512];
    cache_path(path, sizeof(path));
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char tag[32];
    if (fscanf(f, "%31s", tag) != 1 || strcmp(tag, "leaderboard") != 0) {
        fclose(f);
        return -1;
    }

    char label[32];
    if (fscanf(f, "%31s %d", label, &lb->count) != 2 || strcmp(label, "count") != 0) {
        fclose(f);
        return -1;
    }

    lb->entries = calloc(lb->count, sizeof(vortex_contributor));
    for (int i = 0; i < lb->count; i++) {
        if (fscanf(f, "%31s %127s %127s %d %d %d %d %d",
                   label,
                   lb->entries[i].username,
                   lb->entries[i].name,
                   &lb->entries[i].rank,
                   &lb->entries[i].commits,
                   &lb->entries[i].pull_requests,
                   &lb->entries[i].issues,
                   &lb->entries[i].reviews) != 8) {
            break;
        }
    }
    fclose(f);
    return 0;
}

void cache_purge(void)
{
    char path[512];
    cache_path(path, sizeof(path));
    remove(path);
}
