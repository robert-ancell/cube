#include <stdlib.h>
#include <string.h>

#include "cube_define.h"

struct _CubeDefine {
  int ref;
  char *name;
  char *value;
};

CubeDefine *cube_define_new(const char *name, const char *value) {
  CubeDefine *self = malloc(sizeof(CubeDefine));

  self->ref = 1;
  self->name = strdup(name);
  self->value = strdup(value);

  return self;
}

const char *cube_define_get_name(CubeDefine *self) { return self->name; }

const char *cube_define_get_value(CubeDefine *self) { return self->value; }

CubeDefine *cube_define_ref(CubeDefine *self) {
  self->ref++;
  return self;
}

void cube_define_unref(CubeDefine *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->name);
  free(self->value);
  free(self);
}
