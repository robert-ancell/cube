#pragma once

typedef struct _CubeProject CubeProject;

#include "cube_program.h"
#include "cube_program_array.h"

CubeProject *cube_project_new(CubeProgramArray *programs);

CubeProgramArray *cube_project_get_programs(CubeProject *self);

CubeProject *cube_project_ref(CubeProject *self);

void cube_project_unref(CubeProject *self);
