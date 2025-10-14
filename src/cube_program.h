#pragma once

typedef struct _CubeProgram CubeProgram;

#include "string_array.h"

CubeProgram *cube_program_new(const char *name, StringArray *sources,
                             StringArray *modules,
                              StringArray *libraries);

const char *cube_program_get_name(CubeProgram *self);

StringArray *cube_program_get_sources(CubeProgram *self);

StringArray *cube_program_get_modules(CubeProgram *self);

StringArray *cube_program_get_libraries(CubeProgram *self);

CubeProgram *cube_program_ref(CubeProgram *self);

void cube_program_unref(CubeProgram *self);
