#ifndef CACHE_H
#define CACHE_H

int  cache_get(const char *key, char **data);
int  cache_set(const char *key, const char *data);
void cache_purge(void);

#endif
