#include <stdlib.h>

#include "cube_command.h"
#include "string_functions.h"

struct _CubeCommand {
  int ref;
  StringArray *inputs;
  StringArray *args;
  StringArray *outputs;
  char *label;
};

CubeCommand *cube_command_new(StringArray *inputs, StringArray *args,
                              StringArray *outputs, const char *label) {
  return cube_command_new_take(string_array_ref(inputs), string_array_ref(args),
                               string_array_ref(outputs), string_copy(label));
}

CubeCommand *cube_command_new_take(StringArray *inputs, StringArray *args,
                                   StringArray *outputs, char *label) {
  CubeCommand *self = malloc(sizeof(CubeCommand));

  self->ref = 1;
  self->inputs = inputs;
  self->args = args;
  self->outputs = outputs;
  self->label = label;

  return self;
}

StringArray *cube_command_get_inputs(CubeCommand *self) { return self->inputs; }

StringArray *cube_command_get_args(CubeCommand *self) { return self->args; }

StringArray *cube_command_get_outputs(CubeCommand *self) {
  return self->outputs;
}

const char *cube_command_get_label(CubeCommand *self) { return self->label; }

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
  free(self->label);
  free(self);
}
