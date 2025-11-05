#include <stdlib.h>
#include <string.h>

#include "string_builder.h"

struct _StringBuilder {
  int ref;
  char *string;
  size_t string_length;
};

// Lazy create the empty string
static void ensure_string(StringBuilder *self) {
  if (self->string == NULL) {
    self->string = malloc(1);
    self->string[0] = '\0';
  }
}

StringBuilder *string_builder_new() {
  StringBuilder *self = malloc(sizeof(StringBuilder));

  self->ref = 1;
  self->string = NULL;
  self->string_length = 0;

  return self;
}

void string_builder_append(StringBuilder *self, const char *string) {
  size_t string_length = strlen(string);
  self->string = realloc(self->string, self->string_length + string_length + 1);
  for (size_t i = 0; i <= string_length; i++) {
    self->string[self->string_length + i] = string[i];
  }
  self->string_length += string_length;
}

void string_builder_append_take(StringBuilder *self, char *string) {
  string_builder_append(self, string);
  free(string);
}

void string_builder_append_codepoint(StringBuilder *self, uint32_t codepoint) {
  if (codepoint <= 0x7f) {
    self->string_length++;
    self->string = realloc(self->string, self->string_length + 1);
    self->string[self->string_length - 1] = codepoint;
    self->string[self->string_length] = '\0';
  } else if (codepoint <= 0x7ff) {
    self->string_length += 2;
    self->string = realloc(self->string, self->string_length + 1);
    self->string[self->string_length - 2] = 0xc0 | (codepoint >> 6);
    self->string[self->string_length - 1] = 0x80 | (codepoint & 0x3f);
    self->string[self->string_length] = '\0';
  } else if (codepoint <= 0xffff) {
    self->string_length += 3;
    self->string = realloc(self->string, self->string_length + 1);
    self->string[self->string_length - 3] = 0xe0 | (codepoint >> 12);
    self->string[self->string_length - 2] = 0x80 | ((codepoint >> 6) & 0xf3);
    self->string[self->string_length - 1] = 0x80 | (codepoint & 0x3f);
    self->string[self->string_length] = '\0';
  } else if (codepoint <= 0x10ffff) {
    self->string_length += 4;
    self->string = realloc(self->string, self->string_length + 1);
    self->string[self->string_length - 4] = 0xf0 | (codepoint >> 18);
    self->string[self->string_length - 3] = 0x80 | ((codepoint >> 12) & 0xf3);
    self->string[self->string_length - 2] = 0x80 | ((codepoint >> 6) & 0xf3);
    self->string[self->string_length - 1] = 0x80 | (codepoint & 0x3f);
    self->string[self->string_length] = '\0';
  } else {
    // FIXME: Invalid codepoint
  }
}

size_t string_builder_get_length(StringBuilder *self) {
  return self->string_length;
}

const char *string_builder_get_string(StringBuilder *self) {
  ensure_string(self);
  return self->string;
}

char *string_builder_take_string(StringBuilder *self) {
  ensure_string(self);
  char *string = self->string;
  self->string = NULL;
  self->string_length = 0;
  return string;
}

StringBuilder *string_builder_ref(StringBuilder *self) {
  self->ref++;
  return self;
}

void string_builder_unref(StringBuilder *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->string);
  free(self);
}
