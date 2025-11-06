#include <assert.h>
#include <stdlib.h>

#include "pointer_set.h"

struct _PointerSet {
  int ref;
  int (*compare_function)(void *, void *);
  void *(*ref_function)(void *);
  void (*unref_function)(void *);
  void **values;
  size_t length;
};

/// Find element with [key] in the set returning `true` if it exists and setting
/// [index].
static bool lookup(PointerSet *self, void *value, size_t *index) {
  // FIXME: Replace with a binary tree
  for (size_t i = 0; i < self->length; i++) {
    if (self->compare_function(self->values[i], value) == 0) {
      *index = i;
      return true;
    }
  }

  return false;
}

PointerSet *pointer_set_new(int (*compare_function)(void *, void *),
                            void *(*ref_function)(void *),
                            void (*unref_function)(void *)) {
  PointerSet *self = malloc(sizeof(PointerSet));

  self->ref = 1;
  self->compare_function = compare_function;
  self->ref_function = ref_function;
  self->unref_function = unref_function;
  self->values = NULL;
  self->length = 0;

  return self;
}

size_t pointer_set_get_length(PointerSet *self) { return self->length; }

void pointer_set_insert(PointerSet *self, void *value) {
  assert(value != NULL);

  pointer_set_insert_take(self, self->ref_function(value));
}

void pointer_set_insert_take(PointerSet *self, void *value) {
  assert(value != NULL);

  size_t index;
  if (lookup(self, value, &index)) {
    void *old_value = self->values[index];
    self->values[index] = value;
    self->unref_function(old_value);
  } else {
    self->length++;
    self->values = realloc(self->values, sizeof(void *) * self->length);
    self->values[self->length - 1] = value;
  }
}

bool pointer_set_contains(PointerSet *self, void *value) {
  size_t index;
  return lookup(self, value, &index);
}

bool pointer_set_remove(PointerSet *self, void *value) {
  size_t index;
  if (!lookup(self, value, &index)) {
    return false;
  }

  self->unref_function(self->values[index]);
  for (size_t i = index; i < self->length - 1; i++) {
    self->values[index] = self->values[index + 1];
  }
  self->length--;
  self->values = realloc(self->values, sizeof(void *) * self->length);

  return true;
}

PointerSet *pointer_set_ref(PointerSet *self) {
  self->ref++;
  return self;
}

void pointer_set_unref(PointerSet *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->length; i++) {
    self->unref_function(self->values[i]);
  }
  free(self->values);
  free(self);
}
