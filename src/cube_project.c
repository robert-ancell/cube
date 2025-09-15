#include <stdlib.h>

#include "cube_project.h"

struct _CubeProject {
  int ref;
  CubeProgram **programs;
  size_t programs_length;
};

CubeProject *cube_project_new() {
  CubeProject *self = malloc(sizeof(CubeProject));

  self->ref = 1;
  self->programs = NULL;
  self->programs_length = 0;

  return self;
}

CubeProject *cube_project_ref(CubeProject *self) {
  self->ref++;
  return self;
}

CubeProgram **cube_project_get_programs(CubeProject *self) {
  return self->programs;
}

void cube_project_unref(CubeProject *self) {
  if (--self->ref == 0) {
    free(self->programs);
    free(self);
  }
}
