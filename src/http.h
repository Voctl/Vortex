#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t len;
} http_response;

int http_get(const char *url, http_response *res);
void http_response_free(http_response *res);

#endif
