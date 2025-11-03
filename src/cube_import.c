#include <stdlib.h>
#include <string.h>

#include "cube_import.h"

struct _CubeImport {
  int ref;
  char *url;
  StringArray *modules;
};

CubeImport *cube_import_new_take(const char *url, StringArray *modules) {
  CubeImport *self = malloc(sizeof(CubeImport));

  self->ref = 1;
  self->url = strdup(url);
  self->modules = modules;

  return self;
}

const char *cube_import_get_url(CubeImport *self) { return self->url; }

StringArray *cube_import_get_modules(CubeImport *self) { return self->modules; }

CubeImport *cube_import_ref(CubeImport *self) {
  self->ref++;
  return self;
}

void cube_import_unref(CubeImport *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->url);
  string_array_unref(self->modules);
  free(self);
}
