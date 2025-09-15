#pragma once

typedef struct _JsonParser JsonParser;

typedef enum {
  JSON_PARSER_ERROR_NONE,
  JSON_PARSER_ERROR_INVALID_CODEPOINT,
  JSON_PARSER_ERROR_MISSING_MEMBER_DIVIDER,
  JSON_PARSER_ERROR_INVALID_MEMBER_NAME,
  JSON_PARSER_ERROR_INVALID_MEMBER_VALUE,
  JSON_PARSER_ERROR_UNTERMINATED_OBJECT,
  JSON_PARSER_ERROR_INVALID_ARRAY_ELEMENT,
  JSON_PARSER_ERROR_UNTERMINATED_ARRAY,
  JSON_PARSER_ERROR_INVALID_STRING_ESCAPE,
  JSON_PARSER_ERROR_UNTERMINATED_STRING,
  JSON_PARSER_ERROR_INVALID_VALUE,
} JsonParserError;

#include "json_value.h"

JsonParser *json_parser_new(const char *text);

JsonParserError json_parser_get_error(JsonParser *self);

JsonValue *json_parser_get_json(JsonParser *self);

void json_parser_unref(JsonParser *self);
