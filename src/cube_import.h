#pragma once

typedef struct _CubeImport CubeImport;

#include <cube/array.h>

CubeImport *cube_import_new_take(const char *url, StringArray *modules);

const char *cube_import_get_url(CubeImport *self);

StringArray *cube_import_get_modules(CubeImport *self);

CubeImport *cube_import_ref(CubeImport *self);

void cube_import_unref(CubeImport *self);
