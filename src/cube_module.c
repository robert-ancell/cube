#include <stdlib.h>
#include <string.h>

#include "cube_module.h"

struct _CubeModule {
  int ref;
  char *name;
  StringArray *sources;
  StringArray *modules;
  StringArray *include_directories;
};

CubeModule *cube_module_new_take(const char *name, StringArray *sources,
                                 StringArray *modules,
                                 StringArray *include_directories) {
  CubeModule *self = malloc(sizeof(CubeModule));

  self->ref = 1;
  self->name = strdup(name);
  self->sources = sources;
  self->modules = modules;
  self->include_directories = include_directories;

  return self;
}

const char *cube_module_get_name(CubeModule *self) { return self->name; }

StringArray *cube_module_get_sources(CubeModule *self) { return self->sources; }

StringArray *cube_module_get_modules(CubeModule *self) { return self->modules; }

StringArray *cube_module_get_include_directories(CubeModule *self) {
  return self->include_directories;
}

CubeModule *cube_module_ref(CubeModule *self) {
  self->ref++;
  return self;
}

void cube_module_unref(CubeModule *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->name);
  string_array_unref(self->sources);
  string_array_unref(self->modules);
  string_array_unref(self->include_directories);
  free(self);
}
