#pragma once

typedef struct _CubeCommand CubeCommand;

#include <stdbool.h>
#include <unistd.h>

CubeCommand *cube_command_new(const char **inputs, size_t inputs_length,
                              const char **args, size_t args_length,
                              const char **outputs, size_t outputs_length);

char **cube_command_get_args(CubeCommand *self);

CubeCommand *cube_command_ref(CubeCommand *self);

void cube_command_unref(CubeCommand *self);
