#include <stdlib.h>
#include <string.h>

#include "cube_program.h"

struct _CubeProgram {
  int ref;
  char *name;
  StringArray *sources;
};

CubeProgram *cube_program_new(const char *name, StringArray *sources) {
  CubeProgram *self = malloc(sizeof(CubeProgram));

  self->ref = 1;
  self->name = strdup(name);
  self->sources = string_array_ref(sources);

  return self;
}

const char *cube_program_get_name(CubeProgram *self) { return self->name; }

StringArray *cube_program_get_sources(CubeProgram *self) {
  return self->sources;
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
  free(self);
}
