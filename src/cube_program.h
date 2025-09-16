#pragma once

typedef struct _CubeProgram CubeProgram;

CubeProgram *cube_program_new(const char *name, const char **sources,
                              size_t sources_length);

const char *cube_program_get_name(CubeProgram *self);

char **cube_program_get_sources(CubeProgram *self, size_t *sources_length);

CubeProgram *cube_program_ref(CubeProgram *self);

void cube_program_unref(CubeProgram *self);
