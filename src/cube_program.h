#pragma once

typedef struct _CubeProgram CubeProgram;

#include "string_array.h"

CubeProgram *cube_program_new(const char *name, StringArray *sources,
                              StringArray *libraries);

const char *cube_program_get_name(CubeProgram *self);

StringArray *cube_program_get_sources(CubeProgram *self);

StringArray *cube_program_get_libraries(CubeProgram *self);

CubeProgram *cube_program_ref(CubeProgram *self);

void cube_program_unref(CubeProgram *self);
