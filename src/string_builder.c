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
