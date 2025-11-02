#pragma once

typedef struct _CubeCommand CubeCommand;

#include <cube/array.h>

CubeCommand *cube_command_new(StringArray *inputs, StringArray *args,
                              StringArray *outputs, const char *label);

CubeCommand *cube_command_new_take(StringArray *inputs, StringArray *args,
                                   StringArray *outputs, char *label);

StringArray *cube_command_get_inputs(CubeCommand *self);

StringArray *cube_command_get_args(CubeCommand *self);

StringArray *cube_command_get_outputs(CubeCommand *self);

const char *cube_command_get_label(CubeCommand *self);

CubeCommand *cube_command_ref(CubeCommand *self);

void cube_command_unref(CubeCommand *self);
