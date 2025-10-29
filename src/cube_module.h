#pragma once

typedef struct _CubeModule CubeModule;

#include "string_array.h"

CubeModule *cube_module_new(const char *name, StringArray *sources,
                            StringArray *modules,
                            StringArray *include_directories);

const char *cube_module_get_name(CubeModule *self);

StringArray *cube_module_get_sources(CubeModule *self);

StringArray *cube_module_get_modules(CubeModule *self);

StringArray *cube_module_get_include_directories(CubeModule *self);

CubeModule *cube_module_ref(CubeModule *self);

void cube_module_unref(CubeModule *self);
