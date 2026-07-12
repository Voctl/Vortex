#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int http_get(const char *url, http_response *res)
{
    memset(res, 0, sizeof(*res));

    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "curl -s -L --max-time 15 '%s' 2>/dev/null", url);

    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;

    char buf[16384];
    size_t total = 0;

    while (1) {
        size_t n = fread(buf, 1, sizeof(buf), fp);
        if (n == 0) break;
        res->data = realloc(res->data, total + n + 1);
        memcpy(res->data + total, buf, n);
        total += n;
        res->data[total] = '\0';
    }

    int status = pclose(fp);
    if (status != 0 || total == 0) {
        http_response_free(res);
        return -1;
    }

    res->len = total;
    return 0;
}

void http_response_free(http_response *res)
{
    free(res->data);
    res->data = NULL;
    res->len = 0;
}
