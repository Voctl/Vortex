#include "api.h"
#include "http.h"
#include "json.h"
#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int fetch_json(const char *url, const char *cache_key, char **out)
{
    if (cache_key && cache_get(cache_key, out) == 0)
        return 0;

    http_response res;
    if (http_get(url, &res) != 0) return -1;

    *out = strdup(res.data);
    if (cache_key)
        cache_set(cache_key, res.data);
    http_response_free(&res);
    return 0;
}

static int parse_entries(json_value *arr, vortex_leaderboard *lb)
{
    int n = json_array_len(arr);
    if (n > 100) n = 100;

    lb->count = n;
    lb->entries = calloc(n, sizeof(vortex_contributor));

    for (int i = 0; i < n; i++) {
        json_value *entry = json_array_get(arr, i);
        json_value *user = json_object_get(entry, "user");
        json_value *val;

        if (user) {
            if ((val = json_object_get(user, "githubUsername")))
                snprintf(lb->entries[i].username, sizeof(lb->entries[i].username),
                         "%s", json_as_string(val));
            if ((val = json_object_get(user, "name")))
                snprintf(lb->entries[i].name, sizeof(lb->entries[i].name),
                         "%s", json_as_string(val));
        }

        lb->entries[i].rank = i + 1;
        if ((val = json_object_get(entry, "commits")))
            lb->entries[i].commits = json_as_int(val);
        if ((val = json_object_get(entry, "pullRequests")))
            lb->entries[i].pull_requests = json_as_int(val);
        if ((val = json_object_get(entry, "issues")))
            lb->entries[i].issues = json_as_int(val);
        if ((val = json_object_get(entry, "reviews")))
            lb->entries[i].reviews = json_as_int(val);
    }
    return 0;
}

static int fetch_leaderboard(const char *url, const char *cache_key, vortex_leaderboard *lb)
{
    char *raw = NULL;
    if (fetch_json(url, cache_key, &raw) != 0) return -1;

    json_value *root = json_parse(raw);
    free(raw);
    if (!root || root->type != JSON_OBJECT) { json_free(root); return -1; }

    json_value *data = json_object_get(root, "data");
    if (!data || data->type != JSON_ARRAY) { json_free(root); return -1; }

    int ret = parse_entries(data, lb);
    json_free(root);
    return ret;
}

static int gca_fetch_alltime(vortex_leaderboard *lb)
{
    return fetch_leaderboard(API_BASE_URL "/leaderboard/all-time", "leaderboard_all", lb);
}

static int gca_fetch_monthly(vortex_leaderboard *lb)
{
    return fetch_leaderboard(API_BASE_URL "/leaderboard", "leaderboard", lb);
}

static int lookup_user(vortex_leaderboard *lb, const char *username)
{
    for (int i = 0; i < lb->count; i++) {
        if (strcmp(lb->entries[i].username, username) == 0)
            return i;
    }
    return -1;
}

static int gca_fetch_status(const char *username, vortex_user_status *st)
{
    vortex_leaderboard monthly = {0}, alltime = {0};
    int found = -1, is_monthly = 1;

    if (gca_fetch_monthly(&monthly) == 0)
        found = lookup_user(&monthly, username);

    if (found < 0 && gca_fetch_alltime(&alltime) == 0) {
        found = lookup_user(&alltime, username);
        is_monthly = 0;
    }

    vortex_contributor *entry = NULL;
    int total = 0;
    if (found >= 0 && is_monthly) {
        entry = &monthly.entries[found];
        total = monthly.count;
    } else if (found >= 0) {
        entry = &alltime.entries[found];
        total = alltime.count;
    }

    if (entry) {
        snprintf(st->username, sizeof(st->username), "%s", entry->username);
        snprintf(st->name, sizeof(st->name), "%s", entry->name);
        st->rank = entry->rank;
        st->total_contributors = total;
        st->commits_week = entry->commits;
        st->pull_requests = entry->pull_requests;
        st->issues = entry->issues;
        st->reviews = entry->reviews;
        if (!is_monthly) st->commits_today = -1;
    } else {
        snprintf(st->username, sizeof(st->username), "%s", username);
    }

    free(monthly.entries);
    free(alltime.entries);
    return entry ? 0 : -1;
}

static int gca_fetch_news(vortex_news *news)
{
    char *raw = NULL;
    if (fetch_json(API_BASE_URL "/blog", "blog", &raw) != 0) return -1;

    json_value *root = json_parse(raw);
    free(raw);
    if (!root || root->type != JSON_OBJECT) { json_free(root); return -1; }

    json_value *data = json_object_get(root, "data");
    if (!data || data->type != JSON_ARRAY) { json_free(root); return -1; }

    int n = json_array_len(data);
    if (n > 10) n = 10;
    news->count = n;
    news->items = calloc(n, sizeof(vortex_news_item));

    for (int i = 0; i < n; i++) {
        json_value *item = json_array_get(data, i);
        json_value *val;

        if ((val = json_object_get(item, "title")))
            snprintf(news->items[i].title, sizeof(news->items[i].title),
                     "%s", json_as_string(val));

        if ((val = json_object_get(item, "createdAt"))) {
            const char *d = json_as_string(val);
            if (d) {
                size_t dlen = strlen(d);
                if (dlen > 10) dlen = 10;
                memcpy(news->items[i].date, d, dlen);
                news->items[i].date[dlen] = '\0';
            }
        }
    }

    json_free(root);
    return news->count > 0 ? 0 : -1;
}

static void gen_demo_news(vortex_news *news)
{
    const char *titles[] = {
        "Sprint #12: Memory allocator optimization merged",
        "KhazarOS reaches 500 stars on GitHub",
    };
    const char *dates[] = {
        "2026-07-10", "2026-07-08",
    };
    int n = sizeof(titles) / sizeof(titles[0]);
    news->count = n;
    news->items = calloc(n, sizeof(vortex_news_item));
    for (int i = 0; i < n; i++) {
        snprintf(news->items[i].title, sizeof(news->items[i].title), "%s", titles[i]);
        snprintf(news->items[i].date, sizeof(news->items[i].date), "%s", dates[i]);
    }
}

int api_fetch_leaderboard(const vortex_config *cfg, vortex_leaderboard *lb)
{
    (void)cfg;
    if (gca_fetch_alltime(lb) == 0) return 0;
    lb->count = 0; lb->entries = NULL; return -1;
}

int api_fetch_monthly_leaderboard(const vortex_config *cfg, vortex_leaderboard *lb)
{
    (void)cfg;
    if (gca_fetch_monthly(lb) == 0) return 0;
    lb->count = 0; lb->entries = NULL; return -1;
}

int api_fetch_status(const vortex_config *cfg, const char *username, vortex_user_status *st)
{
    (void)cfg;
    memset(st, 0, sizeof(*st));
    if (gca_fetch_status(username, st) == 0) return 0;
    snprintf(st->username, sizeof(st->username), "%s", username);
    st->rank = 0;
    st->total_contributors = 0;
    return -1;
}

int api_fetch_news(const vortex_config *cfg, vortex_news *news)
{
    (void)cfg;
    if (gca_fetch_news(news) == 0 && news->count > 0) return 0;
    gen_demo_news(news);
    return 0;
}
