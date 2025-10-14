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
