#include <stdlib.h>
#include <string.h>

#include "utf8.h"

static bool is_continuation_byte(char c) { return (c & 0xc0) == 0x80; }

uint32_t utf8_read_codepoint(const char *text, size_t *length) {
  uint32_t c = text[0];
  if ((c & 0x80) == 0) {
    *length = 1;
    return c;
  } else if ((c & 0xe0) == 0xc0) {
    if (!is_continuation_byte(text[1])) {
      *length = 0;
      return 0;
    }
    c = (c & 0x1f) << 6 | (text[1] & 0x3f);
    *length = 2;
    return c;
  } else if ((c & 0xf0) == 0xe0) {
    if (!is_continuation_byte(text[1]) || !is_continuation_byte(text[2])) {
      *length = 0;
      return 0;
    }
    c = (c & 0x0f) << 12 | (text[1] & 0x3f) << 6 | (text[2] & 0x3f);
    *length = 3;
    return c;
  } else if ((c & 0xf8) == 0xf0) {
    if (!is_continuation_byte(text[1]) || !is_continuation_byte(text[2]) ||
        !is_continuation_byte(text[3])) {
      *length = 0;
      return 0;
    }
    c = (c & 0x07) << 18 | (text[1] & 0x3f) << 12 | (text[2] & 0x3f) << 6 |
        (text[3] & 0x3f);
    *length = 4;
    return c;
  } else {
    *length = 0;
    return 0;
  }
}

char *utf8_append_codepoint(char *string, uint32_t codepoint) {
  size_t string_length = strlen(string);
  if (codepoint <= 0x7f) {
    string_length++;
    string = realloc(string, string_length + 1);
    string[string_length - 1] = codepoint;
  } else if (codepoint <= 0x7ff) {
    string_length += 2;
    string = realloc(string, string_length + 1);
    string[string_length - 2] = 0xc0 | (codepoint >> 6);
    string[string_length - 1] = 0x80 | (codepoint & 0x3f);
  } else if (codepoint <= 0xffff) {
    string_length += 3;
    string = realloc(string, string_length + 1);
    string[string_length - 3] = 0xe0 | (codepoint >> 12);
    string[string_length - 2] = 0x80 | ((codepoint >> 6) & 0xf3);
    string[string_length - 1] = 0x80 | (codepoint & 0x3f);
  } else if (codepoint <= 0x10ffff) {
    string_length += 4;
    string = realloc(string, string_length + 1);
    string[string_length - 4] = 0xf0 | (codepoint >> 18);
    string[string_length - 3] = 0x80 | ((codepoint >> 12) & 0xf3);
    string[string_length - 2] = 0x80 | ((codepoint >> 6) & 0xf3);
    string[string_length - 1] = 0x80 | (codepoint & 0x3f);
  } else {
    return NULL;
  }
  string[string_length] = '\0';

  return string;
}
