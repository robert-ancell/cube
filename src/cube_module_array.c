#include <stdlib.h>

#include <cube/array.h>

#include "cube_module_array.h"

struct _CubeModuleArray {
  int ref;
  PointerArray *array;
};

static void *ref_module(void *module) { return cube_module_ref(module); }

static void unref_module(void *module) { cube_module_unref(module); }

CubeModuleArray *cube_module_array_new() {
  CubeModuleArray *self = malloc(sizeof(CubeModuleArray));

  self->ref = 1;
  self->array = pointer_array_new(ref_module, unref_module);

  return self;
}

void cube_module_array_append(CubeModuleArray *self, CubeModule *module) {
  pointer_array_append(self->array, module);
}

void cube_module_array_append_take(CubeModuleArray *self, CubeModule *module) {
  pointer_array_append_take(self->array, module);
}

size_t cube_module_array_get_length(CubeModuleArray *self) {
  return pointer_array_get_length(self->array);
}

CubeModule *cube_module_array_get_element(CubeModuleArray *self, size_t i) {
  return pointer_array_get_element(self->array, i);
}

CubeModuleArray *cube_module_array_ref(CubeModuleArray *self) {
  self->ref++;
  return self;
}

void cube_module_array_unref(CubeModuleArray *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_array_unref(self->array);
  free(self);
}
