#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "json_parser.h"
#include "utf8.h"

struct _JsonParser {
  const char *text;
  size_t offset;
  JsonParserError error;
  JsonValue *json;
};

static void set_error(JsonParser *self, JsonParserError error) {
  if (self->error != JSON_PARSER_ERROR_NONE) {
    return;
  }

  self->error = error;
}

static bool parse_constant(JsonParser *self, const char *text) {
  size_t text_length = strlen(text);
  if (strncmp(self->text + self->offset, text, text_length) == 0) {
    self->offset += text_length;
    return true;
  }

  return false;
}

static bool is_whitespace(char c) {
  return c == 0x20 || c == 0x0a || c == 0x0d || c == 0x09;
}

static void parse_whitespace(JsonParser *self) {
  while (is_whitespace(self->text[self->offset])) {
    self->offset++;
  }
}

static JsonValue *parse_string(JsonParser *self) {
  if (!parse_constant(self, "\"")) {
    return NULL;
  }

  char *string = strdup("");
  while (self->text[self->offset] != '\0' && self->text[self->offset] != '\"') {
    uint32_t c;
    if (self->text[self->offset] == '\\') {
      self->offset++;
      switch (self->text[self->offset]) {
      case '"':
        self->offset++;
        c = '"';
        break;
      case '\\':
        self->offset++;
        c = '\\';
        break;
      case '/':
        self->offset++;
        c = '/';
        break;
      case 'b':
        self->offset++;
        c = '\b';
        break;
      case 'f':
        self->offset++;
        c = '\f';
        break;
      case 'n':
        self->offset++;
        c = '\n';
        break;
      case 'r':
        self->offset++;
        c = '\r';
        break;
      case 't':
        self->offset++;
        c = '\t';
        break;
      case 'u':
        self->offset++;
        c = 0;
        for (size_t i = 0; i < 4; i++) {
          char hex = self->text[self->offset];
          if (hex >= '0' && hex <= '9') {
            hex -= '0';
          } else if (hex >= 'a' && hex <= 'f') {
            hex -= 'a';
          } else if (hex >= 'A' && hex <= 'F') {
            hex -= 'A';
          } else {
            set_error(self, JSON_PARSER_ERROR_INVALID_STRING_ESCAPE);
            free(string);
            return NULL;
          }
          c = c << 8 | hex;
          self->offset++;
        }
        break;
      default:
        set_error(self, JSON_PARSER_ERROR_INVALID_STRING_ESCAPE);
        free(string);
        return NULL;
      }
    } else {
      size_t c_length;
      c = utf8_read_codepoint(self->text + self->offset, &c_length);
      if (c == 0) {
        set_error(self, JSON_PARSER_ERROR_INVALID_CODEPOINT);
        free(string);
        return NULL;
      }
      self->offset += c_length;
    }
    string = utf8_append_codepoint(string, c);
    assert(string != NULL);
  }

  if (!parse_constant(self, "\"")) {
    set_error(self, JSON_PARSER_ERROR_UNTERMINATED_STRING);
    free(string);
    return NULL;
  }

  return json_value_new_string_take(string);
}

static JsonValue *parse_element(JsonParser *self);

static JsonValue *parse_object(JsonParser *self) {
  if (!parse_constant(self, "{")) {
    return NULL;
  }

  parse_whitespace(self);

  JsonValue *object = json_value_new_object();
  if (parse_constant(self, "}")) {
    return object;
  }

  do {
    parse_whitespace(self);
    JsonValue *name = parse_string(self);
    if (name == NULL) {
      set_error(self, JSON_PARSER_ERROR_INVALID_MEMBER_NAME);
      json_value_free(object);
      return NULL;
    }
    parse_whitespace(self);
    if (!parse_constant(self, ":")) {
      set_error(self, JSON_PARSER_ERROR_MISSING_MEMBER_DIVIDER);
      json_value_free(name);
      json_value_free(object);
      return NULL;
    }
    JsonValue *value = parse_element(self);
    if (value == NULL) {
      set_error(self, JSON_PARSER_ERROR_INVALID_MEMBER_VALUE);
      json_value_free(name);
      json_value_free(object);
      return NULL;
    }

    json_value_set_member(object, json_value_get_string(name), value);
    json_value_free(name);
  } while (parse_constant(self, ","));

  if (!parse_constant(self, "}")) {
    set_error(self, JSON_PARSER_ERROR_UNTERMINATED_OBJECT);
    json_value_free(object);
    return NULL;
  }

  return object;
}

static JsonValue *parse_array(JsonParser *self) {
  if (!parse_constant(self, "[")) {
    return NULL;
  }

  parse_whitespace(self);

  JsonValue *array = json_value_new_array();
  if (parse_constant(self, "]")) {
    return array;
  }

  do {
    JsonValue *element = parse_element(self);
    if (element == NULL) {
      set_error(self, JSON_PARSER_ERROR_INVALID_ARRAY_ELEMENT);
      json_value_free(array);
      return NULL;
    }

    json_value_add_element(array, element);
  } while (parse_constant(self, ","));

  if (!parse_constant(self, "]")) {
    set_error(self, JSON_PARSER_ERROR_UNTERMINATED_ARRAY);
    json_value_free(array);
    return NULL;
  }

  return array;
}

static JsonValue *parse_number(JsonParser *self) { return NULL; }

static JsonValue *parse_true(JsonParser *self) {
  return parse_constant(self, "true") ? json_value_new_true() : NULL;
}

static JsonValue *parse_false(JsonParser *self) {
  return parse_constant(self, "false") ? json_value_new_false() : NULL;
}

static JsonValue *parse_null(JsonParser *self) {
  return parse_constant(self, "null") ? json_value_new_null() : NULL;
}

static JsonValue *parse_value(JsonParser *self) {
  JsonValue *value = parse_object(self);
  if (value == NULL) {
    value = parse_array(self);
  }
  if (value == NULL) {
    value = parse_string(self);
  }
  if (value == NULL) {
    value = parse_number(self);
  }
  if (value == NULL) {
    value = parse_true(self);
  }
  if (value == NULL) {
    value = parse_false(self);
  }
  if (value == NULL) {
    value = parse_null(self);
  }
  if (value == NULL) {
    set_error(self, JSON_PARSER_ERROR_INVALID_VALUE);
    return NULL;
  }

  return value;
}

static JsonValue *parse_element(JsonParser *self) {
  parse_whitespace(self);
  JsonValue *value = parse_value(self);
  if (value == NULL) {
    // FIXME: error
    return NULL;
  }
  parse_whitespace(self);

  return value;
}

JsonParser *json_parser_new(const char *text) {
  JsonParser *self = malloc(sizeof(JsonParser));

  self->text = text;
  self->offset = 0;
  self->error = JSON_PARSER_ERROR_NONE;
  self->json = parse_element(self);

  return self;
}

JsonParserError json_parser_get_error(JsonParser *self) { return self->error; }

JsonValue *json_parser_get_json(JsonParser *self) {
  assert(self->error == JSON_PARSER_ERROR_NONE);
  return self->json;
}

void json_parser_free(JsonParser *self) {
  if (self->json != NULL) {
    json_value_free(self->json);
  }
  free(self);
}
