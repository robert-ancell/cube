#pragma once

typedef struct _CubeCommandArray CubeCommandArray;

#include "cube_command.h"

CubeCommandArray *cube_command_array_new();

void cube_command_array_append(CubeCommandArray *self, CubeCommand *command);

void cube_command_array_append_take(CubeCommandArray *self,
                                    CubeCommand *command);

size_t cube_command_array_get_length(CubeCommandArray *self);

CubeCommand *cube_command_array_get_element(CubeCommandArray *self, size_t i);

CubeCommandArray *cube_command_array_ref(CubeCommandArray *self);

void cube_command_array_unref(CubeCommandArray *self);
