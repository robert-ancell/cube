#include <stdlib.h>

#include "pointer_array.h"

struct _PointerArray {
  int ref;
  void *(*ref_function)(void *);
  void (*unref_function)(void *);
  void **elements;
  size_t elements_length;
};

PointerArray *pointer_array_new(void *(*ref_function)(void *),
                                void (*unref_function)(void *)) {
  PointerArray *self = malloc(sizeof(PointerArray));

  self->ref = 1;
  self->ref_function = ref_function;
  self->unref_function = unref_function;
  self->elements = NULL;
  self->elements_length = 0;

  return self;
}

void pointer_array_append(PointerArray *self, void *element) {
  pointer_array_append_take(self, self->ref_function(element));
}

void pointer_array_append_take(PointerArray *self, void *element) {
  self->elements_length++;
  self->elements =
      realloc(self->elements, sizeof(void *) * self->elements_length);
  self->elements[self->elements_length - 1] = element;
}

size_t pointer_array_get_length(PointerArray *self) {
  return self->elements_length;
}

void *pointer_array_get_element(PointerArray *self, size_t i) {
  if (i >= self->elements_length) {
    return NULL;
  }
  return self->elements[i];
}

PointerArray *pointer_array_ref(PointerArray *self) {
  self->ref++;
  return self;
}

void pointer_array_unref(PointerArray *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->elements_length; i++) {
    self->unref_function(self->elements[i]);
  }
  free(self->elements);
  free(self);
}
