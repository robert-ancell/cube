#pragma once

typedef struct _JsonValue JsonValue;

typedef enum {
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_STRING,
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_TRUE,
  JSON_VALUE_TYPE_FALSE,
  JSON_VALUE_TYPE_NULL
} JsonValueType;

JsonValue *json_value_new_object();

JsonValue *json_value_new_array();

JsonValue *json_value_new_string();

JsonValue *json_value_new_number();

JsonValue *json_value_new_true();

JsonValue *json_value_new_false();

JsonValue *json_value_new_null();

JsonValueType json_value_get_type(JsonValue *self);

JsonValue *json_value_get_member(JsonValue *self, const char *name);

size_t json_value_get_length(JsonValue *self);

JsonValue *json_value_get_element(JsonValue *self, size_t index);

const char *json_value_get_string(JsonValue *self);

double json_value_get_number(JsonValue *self);

/// Set [name]/[value] pair in an object.
void json_value_set_member(JsonValue *self, const char *name, JsonValue *value);

/// Add [element] to the end of an array.
void json_value_add_element(JsonValue *self, JsonValue *element);

void json_value_free(JsonValue *self);
