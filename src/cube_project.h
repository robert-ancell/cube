#pragma once

typedef struct _CubeProject CubeProject;

#include "cube_program.h"

CubeProject *cube_project_new();

CubeProject *cube_project_ref(CubeProject *self);

CubeProgram **cube_project_get_programs(CubeProject *self);

void cube_project_unref(CubeProject *self);
