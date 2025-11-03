#include <stdlib.h>

#include <cube/string.h>

#include "cube_project.h"

struct _CubeProject {
  int ref;
  CubeProgramArray *programs;
  CubeImportArray *imports;
  CubeModuleArray *modules;
};

CubeProject *cube_project_new_take(CubeProgramArray *programs,
                                   CubeImportArray *imports,
                                   CubeModuleArray *modules) {
  CubeProject *self = malloc(sizeof(CubeProject));

  self->ref = 1;
  self->programs = programs;
  self->imports = imports;
  self->modules = modules;

  return self;
}

CubeProgramArray *cube_project_get_programs(CubeProject *self) {
  return self->programs;
}

CubeImportArray *cube_project_get_imports(CubeProject *self) {
  return self->imports;
}

CubeModuleArray *cube_project_get_modules(CubeProject *self) {
  return self->modules;
}

CubeModule *cube_project_get_module(CubeProject *self, const char *name) {
  size_t modules_length = cube_module_array_get_length(self->modules);
  for (size_t i = 0; i < modules_length; i++) {
    CubeModule *module = cube_module_array_get_element(self->modules, i);
    if (string_matches(cube_module_get_name(module), name)) {
      return module;
    }
  }

  return NULL;
}

CubeProject *cube_project_ref(CubeProject *self) {
  self->ref++;
  return self;
}

void cube_project_unref(CubeProject *self) {
  if (--self->ref != 0) {
    return;
  }

  cube_program_array_unref(self->programs);
  cube_import_array_unref(self->imports);
  cube_module_array_unref(self->modules);
  free(self);
}
