#include <stdlib.h>
#include <string.h>

#include "cube_program.h"

struct _CubeProgram {
  char *name;
  char **sources;
  size_t sources_length;
};

CubeProgram *cube_program_new(const char *name, const char **sources,
                              size_t sources_length) {
  CubeProgram *self = malloc(sizeof(CubeProgram));

  self->name = strdup(name);
  self->sources = malloc(sizeof(char *) * sources_length);
  for (size_t i = 0; i < sources_length; i++) {
    self->sources[i] = strdup(sources[i]);
  }
  self->sources_length = sources_length;

  return self;
}

const char *cube_program_get_name(CubeProgram *self) { return self->name; }

void cube_program_unref(CubeProgram *self) {
  free(self->name);
  for (size_t i = 0; i < self->sources_length; i++) {
    free(self->sources[i]);
  }
  free(self->sources);
  free(self);
}
