#pragma once

typedef struct _CubeProjectMap CubeProjectMap;

#include <stdbool.h>

#include "cube_project.h"

CubeProjectMap *cube_project_map_new();

size_t cube_project_map_get_length(CubeProjectMap *self);

void cube_project_map_insert(CubeProjectMap *self, const char *key,
                             CubeProject *value);

CubeProject *cube_project_map_lookup(CubeProjectMap *self, const char *key);

void cube_project_map_unref(CubeProjectMap *self);
