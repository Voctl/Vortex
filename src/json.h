#ifndef JSON_H
#define JSON_H

typedef enum {
    JSON_NULL,
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOL
} json_type;

typedef struct json_value json_value;

typedef struct {
    char **keys;
    json_value **values;
    int count;
} json_object;

typedef struct {
    json_value **items;
    int count;
} json_array;

struct json_value {
    json_type type;
    union {
        char *string;
        double number;
        int boolean;
        json_object object;
        json_array array;
    };
};

json_value *json_parse(const char *input);
json_value *json_object_get(const json_value *obj, const char *key);
json_value *json_array_get(const json_value *arr, int index);
int         json_array_len(const json_value *arr);
const char *json_as_string(const json_value *v);
int         json_as_int(const json_value *v);
void        json_free(json_value *v);

#endif
