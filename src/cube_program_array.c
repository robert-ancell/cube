#include <stdlib.h>

#include <cube/array.h>

#include "cube_program_array.h"

struct _CubeProgramArray {
  int ref;
  PointerArray *array;
};

static void *ref_program(void *program) { return cube_program_ref(program); }

static void unref_program(void *program) { cube_program_unref(program); }

CubeProgramArray *cube_program_array_new() {
  CubeProgramArray *self = malloc(sizeof(CubeProgramArray));

  self->ref = 1;
  self->array = pointer_array_new(ref_program, unref_program);

  return self;
}

void cube_program_array_append(CubeProgramArray *self, CubeProgram *program) {
  pointer_array_append(self->array, program);
}

void cube_program_array_append_take(CubeProgramArray *self,
                                    CubeProgram *program) {
  pointer_array_append_take(self->array, program);
}

size_t cube_program_array_get_length(CubeProgramArray *self) {
  return pointer_array_get_length(self->array);
}

CubeProgram *cube_program_array_get_element(CubeProgramArray *self, size_t i) {
  return pointer_array_get_element(self->array, i);
}

CubeProgramArray *cube_program_array_ref(CubeProgramArray *self) {
  self->ref++;
  return self;
}

void cube_program_array_unref(CubeProgramArray *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_array_unref(self->array);
  free(self);
}
