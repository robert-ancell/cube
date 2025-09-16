#include <stdlib.h>
#include <string.h>

#include "cube_program.h"

struct _CubeProgram {
  int ref;
  char *name;
  char **sources;
  size_t sources_length;
};

CubeProgram *cube_program_new(const char *name, char **sources,
                              size_t sources_length) {
  CubeProgram *self = malloc(sizeof(CubeProgram));

  self->ref = 1;
  self->name = strdup(name);
  self->sources = malloc(sizeof(char *) * sources_length);
  for (size_t i = 0; i < sources_length; i++) {
    self->sources[i] = strdup(sources[i]);
  }
  self->sources_length = sources_length;

  return self;
}

const char *cube_program_get_name(CubeProgram *self) { return self->name; }

char **cube_program_get_sources(CubeProgram *self, size_t *sources_length) {
  *sources_length = self->sources_length;
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
  for (size_t i = 0; i < self->sources_length; i++) {
    free(self->sources[i]);
  }
  free(self->sources);
  free(self);
}
