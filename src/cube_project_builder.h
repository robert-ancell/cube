#pragma once

typedef struct _CubeProjectBuilder CubeProjectBuilder;

typedef enum {
  CUBE_PROJECT_BUILDER_ERROR_NONE,
  CUBE_PROJECT_BUILDER_ERROR_NO_PROJECT
} CubeProjectBuilderError;

#include "cube_project.h"

CubeProjectBuilder *cube_project_builder_new();

bool cube_project_builder_run(CubeProjectBuilder *self);

CubeProjectBuilderError
cube_project_builder_get_error(CubeProjectBuilder *self);

void cube_project_builder_unref(CubeProjectBuilder *self);
