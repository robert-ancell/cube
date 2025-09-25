#pragma once

typedef struct _CubeProgramArray CubeProgramArray;

#include "cube_program.h"

CubeProgramArray *cube_program_array_new();

void cube_program_array_append(CubeProgramArray *self, CubeProgram *program);

void cube_program_array_append_take(CubeProgramArray *self,
                                    CubeProgram *program);

size_t cube_program_array_get_length(CubeProgramArray *self);

CubeProgram *cube_program_array_get_element(CubeProgramArray *self, size_t i);

CubeProgramArray *cube_program_array_ref(CubeProgramArray *self);

void cube_program_array_unref(CubeProgramArray *self);
