#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "json_value.h"
#include "utf8.h"

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

static char *append_string(char *string, char *text) {
  size_t string_length = strlen(string);
  size_t text_length = strlen(text);
  string = realloc(string, string_length + text_length + 1);
  for (size_t i = 0; i <= text_length; i++) {
    string[string_length + i] = text[i];
  }
  return string;
}

static char *append_escape(char *string, char escape) {
  string = append_string(string, "\\");
  return utf8_append_codepoint(string, escape);
}

static char *append_hex(char *string, uint32_t value) {
  if (value <= 9) {
    return utf8_append_codepoint(string, '0' + value);
  } else if (value <= 15) {
    return utf8_append_codepoint(string, 'a' + value - 10);
  } else {
    assert(false);
  }
}

static char *append_hex_escape(char *string, uint32_t codepoint) {
  string = append_string(string, "\\u");
  string = append_hex(string, (codepoint >> 24) & 0xff);
  string = append_hex(string, (codepoint >> 16) & 0xff);
  string = append_hex(string, (codepoint >> 8) & 0xff);
  return append_hex(string, codepoint & 0xff);
}

static char *string_to_string(const char *string) {
  char *encoded = strdup("\"");

  size_t offset = 0;
  while (string[offset] != '\0') {
    size_t length;
    uint32_t c = utf8_read_codepoint(string + offset, &length);
    if (length == 0) {
      // FIXME: Invalid codepoint char
      break;
    }

    if (c == '\"' || c == '\\') {
      encoded = append_escape(encoded, c);
    } else if (c == '\b') {
      encoded = append_escape(encoded, 'b');
    } else if (c == '\f') {
      encoded = append_escape(encoded, 'f');
    } else if (c == '\n') {
      encoded = append_escape(encoded, 'n');
    } else if (c == '\r') {
      encoded = append_escape(encoded, 'r');
    } else if (c == '\t') {
      encoded = append_escape(encoded, 't');
    } else if (c < 0x20) {
      encoded = append_hex_escape(encoded, c);
    } else {
      encoded = utf8_append_codepoint(encoded, c);
    }
    offset += length;
  }

  return append_string(encoded, "\"");
}

static char *object_to_string(size_t length, char **names, JsonValue **values) {
  char *string = strdup("{");
  for (size_t i = 0; i < length; i++) {
    if (i != 0) {
      string = append_string(string, ",");
    }
    char *name_string = string_to_string(names[i]);
    char *value_string = json_value_to_string(values[i]);
    string = append_string(string, name_string);
    string = append_string(string, ":");
    string = append_string(string, value_string);
    free(name_string);
    free(value_string);
  }
  return append_string(string, "}");
}

static char *array_to_string(size_t length, JsonValue **elements) {
  char *string = strdup("[");
  for (size_t i = 0; i < length; i++) {
    if (i != 0) {
      string = append_string(string, ",");
    }
    char *element_string = json_value_to_string(elements[i]);
    string = append_string(string, element_string);
    free(element_string);
  }
  return append_string(string, "]");
}

static char *append_integer(char *string, uint64_t integer) {
  uint64_t divisor = 1;
  while (integer / (divisor * 10) != 0) {
    divisor *= 10;
  }
  do {
    uint64_t digit = integer / divisor;
    string = append_hex(string, digit);
    integer -= digit * divisor;
    divisor /= 10;
  } while (divisor != 0);

  return string;
}

static char *append_fraction(char *string, double fraction) {
  if (fraction == 0) {
    return string;
  }

  string = append_string(string, ".");
  while (fraction != 0.0) {
    uint64_t digit = fraction * 10.0;
    string = append_hex(string, digit);
    fraction = fraction * 10.0 - digit;
  }

  return string;
}

static char *append_exponent(char *string, uint64_t exponent) {
  if (exponent == 0) {
    return string;
  }

  string = append_string(string, "e");
  return append_integer(string, exponent);
}

static char *number_to_string(double number) {
  char *string = strdup("");
  double integer;
  double fraction = modf(number, &integer);
  if (integer < 0) {
    string = append_string(string, "-");
    integer = -integer;
  }
  string = append_integer(string, integer);
  string = append_fraction(string, fraction);
  string = append_exponent(string, 0); // FIXME

  return string;
}

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

void json_value_unref(JsonValue *self) {
  switch (self->type) {
  case JSON_VALUE_TYPE_OBJECT:
    for (size_t i = 0; i < self->data.object.length; i++) {
      free(self->data.object.names[i]);
      json_value_unref(self->data.object.values[i]);
    }
    break;
  case JSON_VALUE_TYPE_ARRAY:
    for (size_t i = 0; i < self->data.array.length; i++) {
      json_value_unref(self->data.array.elements[i]);
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
