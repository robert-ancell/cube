#pragma once

typedef struct _CubeModuleArray CubeModuleArray;

#include "cube_module.h"

CubeModuleArray *cube_module_array_new();

void cube_module_array_append(CubeModuleArray *self, CubeModule *module);

void cube_module_array_append_take(CubeModuleArray *self, CubeModule *module);

size_t cube_module_array_get_length(CubeModuleArray *self);

CubeModule *cube_module_array_get_element(CubeModuleArray *self, size_t i);

CubeModuleArray *cube_module_array_ref(CubeModuleArray *self);

void cube_module_array_unref(CubeModuleArray *self);
