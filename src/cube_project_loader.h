#pragma once

typedef struct _CubeProjectLoader CubeProjectLoader;

typedef enum {
  CUBE_PROJECT_LOADER_ERROR_NONE,
  CUBE_PROJECT_LOADER_ERROR_NO_PROJECT,
  CUBE_PROJECT_LOADER_ERROR_INVALID_PROJECT
} CubeProjectLoaderError;

#include "cube_project.h"

CubeProjectLoader *cube_project_loader_new(const char *path);

CubeProjectLoaderError cube_project_loader_get_error(CubeProjectLoader *self);

CubeProject *cube_project_loader_get_project(CubeProjectLoader *self);

void cube_project_loader_unref(CubeProjectLoader *self);
