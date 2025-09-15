#pragma once

typedef struct _CubeProject CubeProject;

#include "cube_program.h"

CubeProject *cube_project_new();

CubeProgram **cube_project_get_programs(CubeProject *self);

void cube_project_unref(CubeProject *self);
