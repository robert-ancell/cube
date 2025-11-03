#pragma once

typedef struct _CubeImportArray CubeImportArray;

#include "cube_import.h"

CubeImportArray *cube_import_array_new();

void cube_import_array_append(CubeImportArray *self, CubeImport *import);

void cube_import_array_append_take(CubeImportArray *self, CubeImport *import);

size_t cube_import_array_get_length(CubeImportArray *self);

CubeImport *cube_import_array_get_element(CubeImportArray *self, size_t i);

CubeImportArray *cube_import_array_ref(CubeImportArray *self);

void cube_import_array_unref(CubeImportArray *self);
