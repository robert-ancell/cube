#include <stdlib.h>

#include "pointer_array.h"

struct _PointerArray {
  int ref;
  void *(*ref_function)(void *);
  void (*unref_function)(void *);
  void **pointers;
  size_t pointers_length;
};

PointerArray *pointer_array_new(void *(*ref_function)(void *),
                                void (*unref_function)(void *)) {
  PointerArray *self = malloc(sizeof(PointerArray));

  self->ref = 1;
  self->ref_function = ref_function;
  self->unref_function = unref_function;
  self->pointers = NULL;
  self->pointers_length = 0;

  return self;
}

void pointer_array_append(PointerArray *self, void *pointer) {
  pointer_array_append_take(self, self->ref_function(pointer));
}

void pointer_array_append_take(PointerArray *self, void *pointer) {
  self->pointers_length++;
  self->pointers =
      realloc(self->pointers, sizeof(void *) * self->pointers_length);
  self->pointers[self->pointers_length - 1] = pointer;
}

size_t pointer_array_get_length(PointerArray *self) {
  return self->pointers_length;
}

void *pointer_array_get_element(PointerArray *self, size_t i) {
  if (i >= self->pointers_length) {
    return NULL;
  }
  return self->pointers[i];
}

PointerArray *pointer_array_ref(PointerArray *self) {
  self->ref++;
  return self;
}

void pointer_array_unref(PointerArray *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->pointers_length; i++) {
    self->unref_function(self->pointers[i]);
  }
  free(self->pointers);
  free(self);
}
