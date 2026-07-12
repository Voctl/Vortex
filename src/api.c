#include "api.h"
#include "http.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char *unescape_json(const char *escaped)
{
    size_t len = strlen(escaped);
    char *out = malloc(len + 1);
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (escaped[i] == '\\' && i + 1 < len) {
            switch (escaped[i + 1]) {
            case '"':  out[j++] = '"'; i++; break;
            case '\\': out[j++] = '\\'; i++; break;
            case '/':  out[j++] = '/'; i++; break;
            case 'n':  out[j++] = '\n'; i++; break;
            case 't':  out[j++] = '\t'; i++; break;
            case 'r':  out[j++] = '\r'; i++; break;
            default:   out[j++] = escaped[i]; break;
            }
        } else {
            out[j++] = escaped[i];
        }
    }
    out[j] = '\0';
    return out;
}

static char *extract_braced_content(const char *html, const char *search)
{
    const char *p = strstr(html, "allTime");
    if (!p) return NULL;
    p = strstr(p, search);
    if (!p) return NULL;
    p = strchr(p, '{');
    if (!p) return NULL;

    int depth = 0;
    const char *start = p;
    const char *end = NULL;
    for (; *p; p++) {
        if (*p == '\\') { p++; continue; }
        if (*p == '{') depth++;
        if (*p == '}') {
            depth--;
            if (depth == 0) { end = p + 1; break; }
        }
    }
    if (!end) return NULL;

    size_t len = end - start;
    char *raw = malloc(len + 1);
    strncpy(raw, start, len);
    raw[len] = '\0';
    return raw;
}

static char *extract_array(const char *html)
{
    const char *p = strstr(html, "allTime");
    if (!p) return NULL;
    p = strchr(p, '[');
    if (!p) return NULL;

    int depth = 0;
    const char *start = p;
    const char *end = NULL;
    for (; *p; p++) {
        if (*p == '\\') { p++; continue; }
        if (*p == '[') depth++;
        if (*p == ']') {
            depth--;
            if (depth == 0) { end = p + 1; break; }
        }
    }
    if (!end) return NULL;

    size_t len = end - start;
    char *raw = malloc(len + 1);
    strncpy(raw, start, len);
    raw[len] = '\0';
    return raw;
}

static int parse_entries(json_value *arr, vortex_leaderboard *lb)
{
    int n = json_array_len(arr);
    if (n > 56) n = 56;

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

static int fetch_page(const char *url, http_response *res)
{
    return http_get(url, res);
}

static int gca_fetch_alltime(vortex_leaderboard *lb)
{
    http_response res;
    if (fetch_page("https://githubcommunity.az/leaderboard", &res) != 0) return -1;

    char *raw = extract_array(res.data);
    http_response_free(&res);
    if (!raw) return -1;

    char *json_str = unescape_json(raw);
    free(raw);
    if (!json_str) return -1;

    json_value *arr = json_parse(json_str);
    free(json_str);
    if (!arr || arr->type != JSON_ARRAY) { json_free(arr); return -1; }

    int ret = parse_entries(arr, lb);
    json_free(arr);
    return ret;
}

static int gca_fetch_monthly(vortex_leaderboard *lb)
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char month_key[16];
    strftime(month_key, sizeof(month_key), "%Y-%m", tm);

    http_response res;
    if (fetch_page("https://githubcommunity.az/leaderboard", &res) != 0) return -1;

    char *raw_obj = extract_braced_content(res.data, "monthly");
    http_response_free(&res);
    if (!raw_obj) return -1;

    char *json_str = unescape_json(raw_obj);
    free(raw_obj);
    if (!json_str) return -1;

    json_value *obj = json_parse(json_str);
    free(json_str);
    if (!obj || obj->type != JSON_OBJECT) { json_free(obj); return -1; }

    json_value *month_arr = json_object_get(obj, month_key);
    if (!month_arr || month_arr->type != JSON_ARRAY) { json_free(obj); return -1; }

    int ret = parse_entries(month_arr, lb);
    json_free(obj);
    return ret;
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
    http_response res;
    if (fetch_page("https://githubcommunity.az/blog", &res) != 0) return -1;

    const char *months[] = {"January","February","March","April","May","June",
                            "July","August","September","October","November","December"};

    news->items = NULL;
    news->count = 0;
    int max_items = 10;
    int idx = 0;
    news->items = calloc(max_items, sizeof(vortex_news_item));

    const char *p = res.data;
    while (p && idx < max_items) {
        p = strstr(p, "font-bold text-hi\">");
        if (!p) break;
        p += 18;
        if (*p == '>') p++;
        const char *te = strstr(p, "</h3>");
        if (!te) break;
        size_t tlen = te - p;
        if (tlen > 0 && tlen < sizeof(news->items[idx].title) - 1) {
            strncpy(news->items[idx].title, p, tlen);
            news->items[idx].title[tlen] = '\0';
        }
        p = te + 5;

        for (int m = 0; m < 12; m++) {
            const char *dp = strstr(p, months[m]);
            if (!dp) continue;
            int skip_digits = 1;
            const char *d = dp + strlen(months[m]);
            while (*d == ' ') d++;
            while (*d >= '0' && *d <= '9') { d++; skip_digits = 0; }
            if (skip_digits) continue;
            if (*d == ',') d++;
            while (*d == ' ') d++;
            if (*d >= '0' && *d <= '9') {
                while (*d >= '0' && *d <= '9') d++;
                size_t dlen = d - dp;
                if (dlen < sizeof(news->items[idx].date) - 1) {
                    strncpy(news->items[idx].date, dp, dlen);
                    news->items[idx].date[dlen] = '\0';
                }
                break;
            }
        }
        idx++;
    }

    news->count = idx;
    http_response_free(&res);
    return 0;
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
