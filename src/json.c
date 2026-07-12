#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static void skip_ws(const char **p)
{
    while (**p && (isspace((unsigned char)**p))) (*p)++;
}

static json_value *parse_value(const char **p);

static char *parse_string(const char **p)
{
    if (**p != '"') return NULL;
    (*p)++;
    size_t cap = 256, len = 0;
    char *s = malloc(cap);
    if (!s) return NULL;

    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            switch (**p) {
            case '"':  s[len++] = '"'; break;
            case '\\': s[len++] = '\\'; break;
            case '/':  s[len++] = '/'; break;
            case 'b':  s[len++] = '\b'; break;
            case 'f':  s[len++] = '\f'; break;
            case 'n':  s[len++] = '\n'; break;
            case 'r':  s[len++] = '\r'; break;
            case 't':  s[len++] = '\t'; break;
            case 'u': {
                (*p)++;
                unsigned code = 0;
                for (int i = 0; i < 4; i++) {
                    char c = **p;
                    code <<= 4;
                    if (c >= '0' && c <= '9') code |= c - '0';
                    else if (c >= 'a' && c <= 'f') code |= c - 'a' + 10;
                    else if (c >= 'A' && c <= 'F') code |= c - 'A' + 10;
                    (*p)++;
                }
                (*p)--;
                if (code < 128) s[len++] = code;
                else if (code < 0x800) { s[len++] = 0xC0 | (code >> 6); s[len++] = 0x80 | (code & 0x3F); }
                else { s[len++] = 0xE0 | (code >> 12); s[len++] = 0x80 | ((code >> 6) & 0x3F); s[len++] = 0x80 | (code & 0x3F); }
                break;
            }
            default: s[len++] = **p; break;
            }
        } else {
            s[len++] = **p;
        }
        (*p)++;
        if (len >= cap - 1) { cap *= 2; s = realloc(s, cap); }
    }
    if (**p == '"') (*p)++;
    s[len] = '\0';
    return s;
}

static json_value *parse_object(const char **p)
{
    json_value *v = calloc(1, sizeof(json_value));
    v->type = JSON_OBJECT;
    v->object.keys = NULL;
    v->object.values = NULL;
    v->object.count = 0;

    if (**p == '{') (*p)++;
    skip_ws(p);
    if (**p == '}') { (*p)++; return v; }

    while (**p) {
        skip_ws(p);
        char *key = parse_string(p);
        if (!key) break;
        skip_ws(p);
        if (**p == ':') (*p)++;
        skip_ws(p);
        json_value *val = parse_value(p);
        if (!val) { free(key); break; }

        v->object.keys = realloc(v->object.keys, (v->object.count + 1) * sizeof(char *));
        v->object.values = realloc(v->object.values, (v->object.count + 1) * sizeof(json_value *));
        v->object.keys[v->object.count] = key;
        v->object.values[v->object.count] = val;
        v->object.count++;

        skip_ws(p);
        if (**p == ',') (*p)++;
        else if (**p == '}') { (*p)++; break; }
    }
    return v;
}

static json_value *parse_array(const char **p)
{
    json_value *v = calloc(1, sizeof(json_value));
    v->type = JSON_ARRAY;
    v->array.items = NULL;
    v->array.count = 0;

    if (**p == '[') (*p)++;
    skip_ws(p);
    if (**p == ']') { (*p)++; return v; }

    while (**p) {
        skip_ws(p);
        json_value *val = parse_value(p);
        if (!val) break;
        v->array.items = realloc(v->array.items, (v->array.count + 1) * sizeof(json_value *));
        v->array.items[v->array.count] = val;
        v->array.count++;
        skip_ws(p);
        if (**p == ',') (*p)++;
        else if (**p == ']') { (*p)++; break; }
    }
    return v;
}

static json_value *parse_number(const char **p)
{
    json_value *v = calloc(1, sizeof(json_value));
    v->type = JSON_NUMBER;
    v->number = 0;

    char *end = NULL;
    v->number = strtod(*p, &end);
    if (end > *p) {
        *p = end;
        return v;
    }
    json_free(v);
    return NULL;
}

static json_value *parse_value(const char **p)
{
    skip_ws(p);
    if (!**p) return NULL;

    if (**p == '"') {
        json_value *v = calloc(1, sizeof(json_value));
        v->type = JSON_STRING;
        v->string = parse_string(p);
        return v;
    }
    if (**p == '{')
        return parse_object(p);
    if (**p == '[')
        return parse_array(p);
    if (**p == 't' && strncmp(*p, "true", 4) == 0) {
        json_value *v = calloc(1, sizeof(json_value));
        v->type = JSON_BOOL;
        v->boolean = 1;
        *p += 4;
        return v;
    }
    if (**p == 'f' && strncmp(*p, "false", 5) == 0) {
        json_value *v = calloc(1, sizeof(json_value));
        v->type = JSON_BOOL;
        v->boolean = 0;
        *p += 5;
        return v;
    }
    if (**p == 'n' && strncmp(*p, "null", 4) == 0) {
        json_value *v = calloc(1, sizeof(json_value));
        v->type = JSON_NULL;
        *p += 4;
        return v;
    }
    if (**p == '-' || isdigit((unsigned char)**p))
        return parse_number(p);

    return NULL;
}

json_value *json_parse(const char *input)
{
    if (!input) return NULL;
    return parse_value(&input);
}

json_value *json_object_get(const json_value *obj, const char *key)
{
    if (!obj || obj->type != JSON_OBJECT) return NULL;
    for (int i = 0; i < obj->object.count; i++) {
        if (strcmp(obj->object.keys[i], key) == 0)
            return obj->object.values[i];
    }
    return NULL;
}

json_value *json_array_get(const json_value *arr, int index)
{
    if (!arr || arr->type != JSON_ARRAY) return NULL;
    if (index < 0 || index >= arr->array.count) return NULL;
    return arr->array.items[index];
}

int json_array_len(const json_value *arr)
{
    if (!arr || arr->type != JSON_ARRAY) return 0;
    return arr->array.count;
}

const char *json_as_string(const json_value *v)
{
    if (!v || v->type != JSON_STRING) return NULL;
    return v->string;
}

int json_as_int(const json_value *v)
{
    if (!v) return 0;
    if (v->type == JSON_NUMBER)
        return (int)v->number;
    if (v->type == JSON_STRING)
        return atoi(v->string);
    return 0;
}

static void json_free_value(json_value *v)
{
    if (!v) return;
    switch (v->type) {
    case JSON_STRING:
        free(v->string);
        break;
    case JSON_OBJECT:
        for (int i = 0; i < v->object.count; i++) {
            free(v->object.keys[i]);
            json_free_value(v->object.values[i]);
        }
        free(v->object.keys);
        free(v->object.values);
        break;
    case JSON_ARRAY:
        for (int i = 0; i < v->array.count; i++)
            json_free_value(v->array.items[i]);
        free(v->array.items);
        break;
    default:
        break;
    }
    free(v);
}

void json_free(json_value *v)
{
    json_free_value(v);
}
