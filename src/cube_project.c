#include <stdlib.h>

#include "cube_project.h"

struct _CubeProject {
  int ref;
  CubeProgramArray *programs;
};

CubeProject *cube_project_new(CubeProgramArray *programs) {
  CubeProject *self = malloc(sizeof(CubeProject));

  self->ref = 1;
  self->programs = cube_program_array_ref(programs);

  return self;
}

CubeProgramArray *cube_project_get_programs(CubeProject *self) {
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

  cube_program_array_unref(self->programs);
  free(self);
}
