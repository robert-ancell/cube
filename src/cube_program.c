#include <stdlib.h>
#include <string.h>

#include "cube_program.h"

struct _CubeProgram {
  int ref;
  char *name;
  StringArray *sources;
  StringArray *modules;
  CubeDefineArray *defines;
  StringArray *libraries;
};

CubeProgram *cube_program_new_take(const char *name, StringArray *sources,
                                   StringArray *modules,
                                   CubeDefineArray *defines,
                                   StringArray *libraries) {
  CubeProgram *self = malloc(sizeof(CubeProgram));

  self->ref = 1;
  self->name = strdup(name);
  self->sources = sources;
  self->modules = modules;
  self->defines = defines;
  self->libraries = libraries;

  return self;
}

const char *cube_program_get_name(CubeProgram *self) { return self->name; }

StringArray *cube_program_get_sources(CubeProgram *self) {
  return self->sources;
}

StringArray *cube_program_get_modules(CubeProgram *self) {
  return self->modules;
}

CubeDefineArray *cube_program_get_defines(CubeProgram *self) {
  return self->defines;
}

StringArray *cube_program_get_libraries(CubeProgram *self) {
  return self->libraries;
}

CubeProgram *cube_program_ref(CubeProgram *self) {
  self->ref++;
  return self;
}

void cube_program_unref(CubeProgram *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->name);
  string_array_unref(self->sources);
  string_array_unref(self->modules);
  string_array_unref(self->libraries);
  free(self);
}
