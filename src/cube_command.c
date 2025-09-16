#include <stdlib.h>
#include <string.h>

#include "cube_command.h"

struct _CubeCommand {
  int ref;
  StringArray *inputs;
  StringArray *args;
  StringArray *outputs;
};

CubeCommand *cube_command_new(StringArray *inputs, StringArray *args,
                              StringArray *outputs) {
  CubeCommand *self = malloc(sizeof(CubeCommand));

  self->ref = 1;
  self->inputs = string_array_ref(inputs);
  self->args = string_array_ref(args);
  self->outputs = string_array_ref(outputs);

  return self;
}

StringArray *cube_command_get_inputs(CubeCommand *self) { return self->inputs; }

StringArray *cube_command_get_args(CubeCommand *self) { return self->args; }

StringArray *cube_command_get_outputs(CubeCommand *self) {
  return self->outputs;
}

CubeCommand *cube_command_ref(CubeCommand *self) {
  self->ref++;
  return self;
}

void cube_command_unref(CubeCommand *self) {
  if (--self->ref != 0) {
    return;
  }

  string_array_unref(self->inputs);
  string_array_unref(self->args);
  string_array_unref(self->outputs);
  free(self);
}
