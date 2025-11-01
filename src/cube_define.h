#pragma once

typedef struct _CubeDefine CubeDefine;

CubeDefine *cube_define_new(const char *name, const char *value);

const char *cube_define_get_name(CubeDefine *self);

const char *cube_define_get_value(CubeDefine *self);

CubeDefine *cube_define_ref(CubeDefine *self);

void cube_define_unref(CubeDefine *self);
