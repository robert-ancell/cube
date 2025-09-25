#include <stdlib.h>

#include "cube_command_array.h"
#include "pointer_array.h"

struct _CubeCommandArray {
  int ref;
  PointerArray *array;
};

static void *ref_command(void *command) { return cube_command_ref(command); }

static void unref_command(void *command) { cube_command_unref(command); }

CubeCommandArray *cube_command_array_new() {
  CubeCommandArray *self = malloc(sizeof(CubeCommandArray));

  self->ref = 1;
  self->array = pointer_array_new(ref_command, unref_command);

  return self;
}

void cube_command_array_append(CubeCommandArray *self, CubeCommand *command) {
  pointer_array_append(self->array, command);
}

void cube_command_array_append_take(CubeCommandArray *self,
                                    CubeCommand *command) {
  pointer_array_append_take(self->array, command);
}

size_t cube_command_array_get_length(CubeCommandArray *self) {
  return pointer_array_get_length(self->array);
}

CubeCommand *cube_command_array_get_element(CubeCommandArray *self, size_t i) {
  return pointer_array_get_element(self->array, i);
}

CubeCommandArray *cube_command_array_ref(CubeCommandArray *self) {
  self->ref++;
  return self;
}

void cube_command_array_unref(CubeCommandArray *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_array_unref(self->array);
  free(self);
}
