#pragma once

typedef struct _CubeProject CubeProject;

#include "cube_import_array.h"
#include "cube_module.h"
#include "cube_module_array.h"
#include "cube_program_array.h"

CubeProject *cube_project_new(CubeProgramArray *programs,
                              CubeImportArray *imports,
                              CubeModuleArray *modules);

CubeProgramArray *cube_project_get_programs(CubeProject *self);

CubeImportArray *cube_project_get_imports(CubeProject *self);

CubeModuleArray *cube_project_get_modules(CubeProject *self);

CubeModule *cube_project_get_module(CubeProject *self, const char *name);

CubeProject *cube_project_ref(CubeProject *self);

void cube_project_unref(CubeProject *self);
