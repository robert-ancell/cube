#include <assert.h>
#include <stdlib.h>

#include <cube/string.h>

#include "pointer_set.h"
#include "string_set.h"

struct _StringSet {
  int ref;
  PointerSet *set;
};

static int value_compare(void *value1, void *value2) {
  const char *v1 = value1;
  const char *v2 = value2;

  // Simple binary compare
  size_t i = 0;
  while (true) {
    if (v1[i] == '\0') {
      if (v2[i] == '\0') {
        return 0;
      }
      // v1 shorter than v2
      return -1;
    } else if (v2[i] == '\0') {
      if (v1[i] == '\0') {
        return 0;
      }
      // v1 longer than v2
      return 1;
    }

    int d = v1[i] - v2[i];
    if (d != 0) {
      return d;
    }

    i++;
  }
}

static void *value_ref(void *value) { return string_copy(value); }

static void value_unref(void *value) { free(value); }

StringSet *string_set_new() {
  StringSet *self = malloc(sizeof(StringSet));

  self->ref = 1;
  self->set = pointer_set_new(value_compare, value_ref, value_unref);

  return self;
}

size_t string_set_get_length(StringSet *self) {
  return pointer_set_get_length(self->set);
}

void string_set_insert(StringSet *self, const char *value) {
  pointer_set_insert(self->set, (void *)value);
}

void string_set_insert_take(StringSet *self, const char *value) {
  pointer_set_insert_take(self->set, (void *)value);
}

bool string_set_contains(StringSet *self, const char *value) {
  return pointer_set_contains(self->set, (void *)value);
}

bool string_set_remove(StringSet *self, const char *value) {
  return pointer_set_remove(self->set, (void *)value);
}

StringSet *string_set_ref(StringSet *self) {
  self->ref++;
  return self;
}

void string_set_unref(StringSet *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_set_unref(self->set);
  free(self);
}
