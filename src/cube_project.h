#pragma once

typedef struct _CubeProject CubeProject;

#include "cube_program.h"

CubeProject *cube_project_new(CubeProgram **programs, size_t programs_length);

CubeProgram **cube_project_get_programs(CubeProject *self,
                                        size_t *programs_length);

CubeProject *cube_project_ref(CubeProject *self);

void cube_project_unref(CubeProject *self);
