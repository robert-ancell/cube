#include <stdlib.h>
#include <string.h>

#include "string_array.h"
#include "string_functions.h"

struct _StringArray {
  int ref;
  char **strings;
  size_t strings_length;
};

StringArray *string_array_new() {
  StringArray *self = malloc(sizeof(StringArray));

  self->ref = 1;
  self->strings = NULL;
  self->strings_length = 0;

  return self;
}

void string_array_append(StringArray *self, const char *string) {
  string_array_append_take(self, strdup(string));
}

void string_array_append_take(StringArray *self, char *string) {
  self->strings_length++;
  self->strings = realloc(self->strings, sizeof(char *) * self->strings_length);
  self->strings[self->strings_length - 1] = string;
}

size_t string_array_get_length(StringArray *self) {
  return self->strings_length;
}

const char *string_array_get_element(StringArray *self, size_t i) {
  if (i >= self->strings_length) {
    return NULL;
  }
  return self->strings[i];
}

bool string_array_contains(StringArray *self, const char *string) {
  for (size_t i = 0; i < self->strings_length; i++) {
    if (string_matches(self->strings[i], string)) {
      return true;
    }
  }

  return false;
}

StringArray *string_array_ref(StringArray *self) {
  self->ref++;
  return self;
}

void string_array_unref(StringArray *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->strings_length; i++) {
    free(self->strings[i]);
  }
  free(self->strings);
  free(self);
}
