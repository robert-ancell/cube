#include <stdlib.h>
#include <string.h>

#include "cube_command.h"

struct _CubeCommand {
  int ref;
  char **inputs;
  size_t inputs_length;
  char **args;
  char **outputs;
  size_t outputs_length;
};

CubeCommand *cube_command_new(const char **inputs, size_t inputs_length,
                              const char **args, size_t args_length,
                              const char **outputs, size_t outputs_length) {
  CubeCommand *self = malloc(sizeof(CubeCommand));

  self->ref = 1;
  self->inputs = malloc(sizeof(char *) * inputs_length);
  for (size_t i = 0; i < inputs_length; i++) {
    self->inputs[i] = strdup(inputs[i]);
  }
  self->args = malloc(sizeof(char *) * (args_length + 1));
  for (size_t i = 0; i < args_length; i++) {
    self->args[i] = strdup(args[i]);
  }
  self->args[args_length] = NULL;
  self->outputs = malloc(sizeof(char *) * outputs_length);
  for (size_t i = 0; i < outputs_length; i++) {
    self->outputs[i] = strdup(outputs[i]);
  }

  return self;
}

char **cube_command_get_args(CubeCommand *self) { return self->args; }

CubeCommand *cube_command_ref(CubeCommand *self) {
  self->ref++;
  return self;
}

void cube_command_unref(CubeCommand *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->inputs_length; i++) {
    free(self->inputs[i]);
  }
  free(self->inputs);
  for (size_t i = 0; self->args[i] != NULL; i++) {
    free(self->args[i]);
  }
  for (size_t i = 0; i < self->outputs_length; i++) {
    free(self->outputs[i]);
  }
  free(self->outputs);
  free(self->args);
  free(self);
}
