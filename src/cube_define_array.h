#pragma once

typedef struct _CubeDefineArray CubeDefineArray;

#include "cube_define.h"

CubeDefineArray *cube_define_array_new();

void cube_define_array_append(CubeDefineArray *self, CubeDefine *define);

void cube_define_array_append_take(CubeDefineArray *self, CubeDefine *define);

size_t cube_define_array_get_length(CubeDefineArray *self);

CubeDefine *cube_define_array_get_element(CubeDefineArray *self, size_t index);

CubeDefineArray *cube_define_array_ref(CubeDefineArray *self);

void cube_define_array_unref(CubeDefineArray *self);
