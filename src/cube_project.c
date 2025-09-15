#include <stdlib.h>

#include "cube_project.h"

struct _CubeProject {
  CubeProgram **programs;
  size_t programs_length;
};

CubeProject *cube_project_new() {
  CubeProject *self = malloc(sizeof(CubeProject));

  self->programs = NULL;
  self->programs_length = 0;

  return self;
}

CubeProgram **cube_project_get_programs(CubeProject *self) {
  return self->programs;
}

void cube_project_unref(CubeProject *self) {
  free(self->programs);
  free(self);
}
