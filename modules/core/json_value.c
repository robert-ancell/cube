#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "json_value.h"
#include "string_builder.h"
#include "utf8.h"

struct _JsonValue {
  int ref;
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

static void append_escape(StringBuilder *builder, char escape) {
  string_builder_append(builder, "\\");
  string_builder_append_codepoint(builder, escape);
}

static void append_hex(StringBuilder *builder, uint32_t value) {
  if (value <= 9) {
    string_builder_append_codepoint(builder, '0' + value);
  } else if (value <= 15) {
    string_builder_append_codepoint(builder, 'a' + value - 10);
  } else {
    assert(false);
  }
}

void append_hex_escape(StringBuilder *builder, uint32_t codepoint) {
  string_builder_append(builder, "\\u");
  append_hex(builder, (codepoint >> 24) & 0xff);
  append_hex(builder, (codepoint >> 16) & 0xff);
  append_hex(builder, (codepoint >> 8) & 0xff);
  append_hex(builder, codepoint & 0xff);
}

static char *string_to_string(const char *string) {
  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, "\"");

  size_t offset = 0;
  while (string[offset] != '\0') {
    size_t length;
    uint32_t c = utf8_read_codepoint(string + offset, &length);
    if (length == 0) {
      // FIXME: Invalid codepoint char
      break;
    }

    if (c == '\"' || c == '\\') {
      append_escape(builder, c);
    } else if (c == '\b') {
      append_escape(builder, 'b');
    } else if (c == '\f') {
      append_escape(builder, 'f');
    } else if (c == '\n') {
      append_escape(builder, 'n');
    } else if (c == '\r') {
      append_escape(builder, 'r');
    } else if (c == '\t') {
      append_escape(builder, 't');
    } else if (c < 0x20) {
      append_hex_escape(builder, c);
    } else {
      string_builder_append_codepoint(builder, c);
    }
    offset += length;
  }

  string_builder_append(builder, "\"");

  char *encoded_string = string_builder_take_string(builder);
  string_builder_unref(builder);
  return encoded_string;
}

static char *object_to_string(size_t length, char **names, JsonValue **values) {
  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, "{");
  for (size_t i = 0; i < length; i++) {
    if (i != 0) {
      string_builder_append(builder, ",");
    }
    char *name_string = string_to_string(names[i]);
    char *value_string = json_value_to_string(values[i]);
    string_builder_append(builder, name_string);
    string_builder_append(builder, ":");
    string_builder_append(builder, value_string);
    free(name_string);
    free(value_string);
  }
  string_builder_append(builder, "}");

  char *string = string_builder_take_string(builder);
  string_builder_unref(builder);
  return string;
}

static char *array_to_string(size_t length, JsonValue **elements) {
  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, "[");
  for (size_t i = 0; i < length; i++) {
    if (i != 0) {
      string_builder_append(builder, ",");
    }
    char *element_string = json_value_to_string(elements[i]);
    string_builder_append(builder, element_string);
    free(element_string);
  }
  string_builder_append(builder, "]");

  char *string = string_builder_take_string(builder);
  string_builder_unref(builder);
  return string;
}

void append_integer(StringBuilder *builder, uint64_t integer) {
  uint64_t divisor = 1;
  while (integer / (divisor * 10) != 0) {
    divisor *= 10;
  }
  do {
    uint64_t digit = integer / divisor;
    append_hex(builder, digit);
    integer -= digit * divisor;
    divisor /= 10;
  } while (divisor != 0);
}

void append_fraction(StringBuilder *builder, double fraction) {
  if (fraction == 0) {
    return;
  }

  string_builder_append(builder, ".");
  while (fraction != 0.0) {
    uint64_t digit = fraction * 10.0;
    append_hex(builder, digit);
    fraction = fraction * 10.0 - digit;
  }
}

void append_exponent(StringBuilder *builder, uint64_t exponent) {
  if (exponent == 0) {
    return;
  }

  string_builder_append(builder, "e");
  append_integer(builder, exponent);
}

static char *number_to_string(double number) {
  StringBuilder *builder = string_builder_new();
  double integer;
  double fraction = modf(number, &integer);
  if (integer < 0) {
    string_builder_append(builder, "-");
    integer = -integer;
  }
  append_integer(builder, integer);
  append_fraction(builder, fraction);
  append_exponent(builder, 0); // FIXME

  char *string = string_builder_take_string(builder);
  string_builder_unref(builder);
  return string;
}

static JsonValue *json_value_new(JsonValueType type) {
  JsonValue *self = malloc(sizeof(JsonValue));

  self->ref = 1;
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

JsonValue *json_value_new_string(const char *string) {
  assert(string != NULL);
  return json_value_new_string_take(strdup(string));
}

JsonValue *json_value_new_string_take(char *string) {
  assert(string != NULL);
  JsonValue *value = json_value_new(JSON_VALUE_TYPE_STRING);
  value->data.string = string;
  return value;
}

JsonValue *json_value_new_number(double number) {
  JsonValue *value = json_value_new(JSON_VALUE_TYPE_NUMBER);
  value->data.number = number;
  return value;
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
  assert(name != NULL);
  for (size_t i = 0; i < self->data.object.length; i++) {
    if (strcmp(self->data.object.names[i], name) == 0) {
      return self->data.object.values[i];
    }
  }
  return NULL;
}

const char *json_value_get_member_name(JsonValue *self, size_t index) {
  assert(self->type == JSON_VALUE_TYPE_OBJECT);
  assert(index < self->data.object.length);
  return self->data.object.names[index];
}

JsonValue *json_value_get_member_value(JsonValue *self, size_t index) {
  assert(self->type == JSON_VALUE_TYPE_OBJECT);
  assert(index < self->data.object.length);
  return self->data.object.values[index];
}

JsonValue *json_value_get_object_member(JsonValue *self, const char *name) {
  JsonValue *value = json_value_get_member(self, name);
  if (value == NULL) {
    return NULL;
  }

  if (json_value_get_type(value) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  return value;
}

JsonValue *json_value_get_array_member(JsonValue *self, const char *name) {
  JsonValue *value = json_value_get_member(self, name);
  if (value == NULL) {
    return NULL;
  }

  if (json_value_get_type(value) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  return value;
}

StringArray *json_value_get_string_array_member(JsonValue *self,
                                                const char *name) {
  JsonValue *array = json_value_get_array_member(self, name);
  if (array == NULL) {
    return NULL;
  }
  size_t array_length = json_value_get_length(array);

  StringArray *string_array = string_array_new();
  for (size_t i = 0; i < array_length; i++) {
    JsonValue *element = json_value_get_element(array, i);
    if (json_value_get_type(element) != JSON_VALUE_TYPE_STRING) {
      string_array_unref(string_array);
      return NULL;
    }

    string_array_append(string_array, json_value_get_string(element));
  }

  return string_array;
}

const char *json_value_get_string_member(JsonValue *self, const char *name,
                                         const char *default_value) {
  JsonValue *value = json_value_get_member(self, name);
  if (value == NULL) {
    return default_value;
  }

  if (json_value_get_type(value) != JSON_VALUE_TYPE_STRING) {
    return NULL;
  }

  return json_value_get_string(value);
}

double json_value_get_number_member(JsonValue *self, const char *name,
                                    double default_value) {
  JsonValue *value = json_value_get_member(self, name);
  if (value == NULL) {
    return default_value;
  }

  if (json_value_get_type(value) != JSON_VALUE_TYPE_NUMBER) {
    return 0.0;
  }

  return json_value_get_number(value);
}

bool json_value_get_boolean_member(JsonValue *self, const char *name,
                                   bool default_value) {
  JsonValue *value = json_value_get_member(self, name);
  if (value == NULL) {
    return default_value;
  }

  switch (json_value_get_type(value)) {
  case JSON_VALUE_TYPE_TRUE:
    return true;
  case JSON_VALUE_TYPE_FALSE:
    return false;
  default:
    return false;
  }
}

size_t json_value_get_length(JsonValue *self) {
  assert(self->type == JSON_VALUE_TYPE_ARRAY ||
         self->type == JSON_VALUE_TYPE_OBJECT);
  return self->type == JSON_VALUE_TYPE_ARRAY ? self->data.array.length
                                             : self->data.object.length;
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
  assert(name != NULL);
  assert(value != NULL);
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
  assert(element != NULL);
  self->data.array.length++;
  self->data.array.elements = realloc(
      self->data.array.elements, self->data.array.length * sizeof(JsonValue *));
  self->data.array.elements[self->data.array.length - 1] = element;
}

char *json_value_to_string(JsonValue *self) {
  switch (self->type) {
  case JSON_VALUE_TYPE_OBJECT:
    return object_to_string(self->data.object.length, self->data.object.names,
                            self->data.object.values);
  case JSON_VALUE_TYPE_ARRAY:
    return array_to_string(self->data.array.length, self->data.array.elements);
  case JSON_VALUE_TYPE_STRING:
    return string_to_string(self->data.string);
  case JSON_VALUE_TYPE_NUMBER:
    return number_to_string(self->data.number);
  case JSON_VALUE_TYPE_TRUE:
    return strdup("true");
  case JSON_VALUE_TYPE_FALSE:
    return strdup("false");
  case JSON_VALUE_TYPE_NULL:
    return strdup("null");
  default:
    assert(false);
  }
}

JsonValue *json_value_ref(JsonValue *self) {
  self->ref++;
  return self;
}

void json_value_unref(JsonValue *self) {
  if (--self->ref != 0) {
    return;
  }

  switch (self->type) {
  case JSON_VALUE_TYPE_OBJECT:
    for (size_t i = 0; i < self->data.object.length; i++) {
      free(self->data.object.names[i]);
      json_value_unref(self->data.object.values[i]);
    }
    free(self->data.object.names);
    free(self->data.object.values);
    break;
  case JSON_VALUE_TYPE_ARRAY:
    for (size_t i = 0; i < self->data.array.length; i++) {
      json_value_unref(self->data.array.elements[i]);
    }
    free(self->data.array.elements);
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
