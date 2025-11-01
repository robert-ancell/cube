#include <stdlib.h>

#include "cube_define_array.h"
#include "pointer_array.h"

struct _CubeDefineArray {
  int ref;
  PointerArray *array;
};

static void *ref_define(void *define) { return cube_define_ref(define); }

static void unref_define(void *define) { cube_define_unref(define); }

CubeDefineArray *cube_define_array_new() {
  CubeDefineArray *self = malloc(sizeof(CubeDefineArray));

  self->ref = 1;
  self->array = pointer_array_new(ref_define, unref_define);

  return self;
}

void cube_define_array_append(CubeDefineArray *self, CubeDefine *define) {
  pointer_array_append(self->array, define);
}

void cube_define_array_append_take(CubeDefineArray *self, CubeDefine *define) {
  pointer_array_append_take(self->array, define);
}

size_t cube_define_array_get_length(CubeDefineArray *self) {
  return pointer_array_get_length(self->array);
}

CubeDefine *cube_define_array_get_element(CubeDefineArray *self, size_t i) {
  return pointer_array_get_element(self->array, i);
}

CubeDefineArray *cube_define_array_ref(CubeDefineArray *self) {
  self->ref++;
  return self;
}

void cube_define_array_unref(CubeDefineArray *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_array_unref(self->array);
  free(self);
}
