#pragma once

typedef struct _CubeCommand CubeCommand;

#include "string_array.h"

CubeCommand *cube_command_new(StringArray *inputs, StringArray *args,
                              StringArray *outputs);

StringArray *cube_command_get_inputs(CubeCommand *self);

StringArray *cube_command_get_args(CubeCommand *self);

StringArray *cube_command_get_outputs(CubeCommand *self);

CubeCommand *cube_command_ref(CubeCommand *self);

void cube_command_unref(CubeCommand *self);
