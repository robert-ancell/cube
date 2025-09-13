#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "json_value.h"

struct _JsonValue {
  JsonValueType type;
  union {
    struct {
      size_t length;
      char **names;
      JsonValue **values;
    } object;
    struct {
      size_t length;
      JsonValue **elements;
    } array;
    char *string;
    double number;
  } data;
};

static JsonValue *json_value_new(JsonValueType type) {
  JsonValue *self = malloc(sizeof(JsonValue));

  self->type = type;
  switch (type) {
  case JSON_VALUE_TYPE_OBJECT:
    self->data.object.length = 0;
    self->data.object.names = NULL;
    self->data.object.values = NULL;
    break;
  case JSON_VALUE_TYPE_ARRAY:
    self->data.array.length = 0;
    self->data.array.elements = NULL;
    break;
  case JSON_VALUE_TYPE_STRING:
    self->data.string = NULL;
    break;
  case JSON_VALUE_TYPE_NUMBER:
    self->data.number = 0.0;
    break;
  case JSON_VALUE_TYPE_TRUE:
  case JSON_VALUE_TYPE_FALSE:
  case JSON_VALUE_TYPE_NULL:
    break;
  }

  return self;
}

JsonValue *json_value_new_object() {
  return json_value_new(JSON_VALUE_TYPE_OBJECT);
}

JsonValue *json_value_new_array() {
  return json_value_new(JSON_VALUE_TYPE_ARRAY);
}

JsonValue *json_value_new_string() {
  return json_value_new(JSON_VALUE_TYPE_STRING);
}

JsonValue *json_value_new_number() {
  return json_value_new(JSON_VALUE_TYPE_NUMBER);
}

JsonValue *json_value_new_true() {
  return json_value_new(JSON_VALUE_TYPE_TRUE);
}

JsonValue *json_value_new_false() {
  return json_value_new(JSON_VALUE_TYPE_FALSE);
}

JsonValue *json_value_new_null() {
  return json_value_new(JSON_VALUE_TYPE_NULL);
}

JsonValueType json_value_get_type(JsonValue *self) { return self->type; }

JsonValue *json_value_get_member(JsonValue *self, const char *name) {
  assert(self->type == JSON_VALUE_TYPE_OBJECT);
  for (size_t i = 0; i < self->data.object.length; i++) {
    if (strcmp(self->data.object.names[i], name) == 0) {
      return self->data.object.values[i];
    }
  }
  return NULL;
}

size_t json_value_get_length(JsonValue *self) {
  assert(self->type == JSON_VALUE_TYPE_ARRAY);
  return self->data.array.length;
}

JsonValue *json_value_get_element(JsonValue *self, size_t index) {
  assert(self->type == JSON_VALUE_TYPE_ARRAY);
  assert(index < self->data.array.length);
  return self->data.array.elements[index];
}

const char *json_value_get_string(JsonValue *self) {
  assert(self->type == JSON_VALUE_TYPE_STRING);
  return self->data.string;
}

double json_value_get_number(JsonValue *self) {
  assert(self->type == JSON_VALUE_TYPE_NUMBER);
  return self->data.number;
}

void json_value_set_member(JsonValue *self, const char *name,
                           JsonValue *value) {
  assert(self->type == JSON_VALUE_TYPE_OBJECT);
  self->data.object.length++;
  self->data.object.names = realloc(self->data.object.names,
                                    self->data.object.length * sizeof(char *));
  self->data.object.values = realloc(
      self->data.object.values, self->data.object.length * sizeof(JsonValue *));
  self->data.object.names[self->data.object.length - 1] = strdup(name);
  self->data.object.values[self->data.object.length - 1] = value;
}

void json_value_add_element(JsonValue *self, JsonValue *element) {
  assert(self->type == JSON_VALUE_TYPE_ARRAY);
  self->data.array.length++;
  self->data.array.elements = realloc(
      self->data.array.elements, self->data.array.length * sizeof(JsonValue *));
  self->data.array.elements[self->data.array.length - 1] = element;
}

void json_value_free(JsonValue *self) {
  switch (self->type) {
  case JSON_VALUE_TYPE_OBJECT:
    for (size_t i = 0; i < self->data.object.length; i++) {
      free(self->data.object.names[i]);
      json_value_free(self->data.object.values[i]);
    }
    break;
  case JSON_VALUE_TYPE_ARRAY:
    for (size_t i = 0; i < self->data.array.length; i++) {
      json_value_free(self->data.array.elements[i]);
    }
    break;
  case JSON_VALUE_TYPE_STRING:
    free(self->data.string);
    break;
  case JSON_VALUE_TYPE_NUMBER:
  case JSON_VALUE_TYPE_TRUE:
  case JSON_VALUE_TYPE_FALSE:
  case JSON_VALUE_TYPE_NULL:
    break;
  }

  free(self);
}
