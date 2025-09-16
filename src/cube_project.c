#include <stdlib.h>

#include "cube_project.h"

struct _CubeProject {
  int ref;
  CubeProgram **programs;
  size_t programs_length;
};

CubeProject *cube_project_new(CubeProgram **programs, size_t programs_length) {
  CubeProject *self = malloc(sizeof(CubeProject));

  self->ref = 1;
  self->programs = malloc(sizeof(CubeProgram *) * programs_length);
  for (size_t i = 0; i < programs_length; i++) {
    self->programs[i] = cube_program_ref(programs[i]);
  }
  self->programs_length = programs_length;

  return self;
}

CubeProgram **cube_project_get_programs(CubeProject *self,
                                        size_t *programs_length) {
  *programs_length = self->programs_length;
  return self->programs;
}

CubeProject *cube_project_ref(CubeProject *self) {
  self->ref++;
  return self;
}

void cube_project_unref(CubeProject *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->programs_length; i++) {
    cube_program_unref(self->programs[i]);
  }
  free(self->programs);
  free(self);
}
