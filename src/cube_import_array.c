#include <stdlib.h>

#include <cube/array.h>

#include "cube_import_array.h"

struct _CubeImportArray {
  int ref;
  PointerArray *array;
};

static void *ref_import(void *import) { return cube_import_ref(import); }

static void unref_import(void *import) { cube_import_unref(import); }

CubeImportArray *cube_import_array_new() {
  CubeImportArray *self = malloc(sizeof(CubeImportArray));

  self->ref = 1;
  self->array = pointer_array_new(ref_import, unref_import);

  return self;
}

void cube_import_array_append(CubeImportArray *self, CubeImport *import) {
  pointer_array_append(self->array, import);
}

void cube_import_array_append_take(CubeImportArray *self, CubeImport *import) {
  pointer_array_append_take(self->array, import);
}

size_t cube_import_array_get_length(CubeImportArray *self) {
  return pointer_array_get_length(self->array);
}

CubeImport *cube_import_array_get_element(CubeImportArray *self, size_t i) {
  return pointer_array_get_element(self->array, i);
}

CubeImportArray *cube_import_array_ref(CubeImportArray *self) {
  self->ref++;
  return self;
}

void cube_import_array_unref(CubeImportArray *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_array_unref(self->array);
  free(self);
}
