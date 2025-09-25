#include <stdio.h>
#include <stdlib.h>

#include "cube_project_loader.h"
#include "json_parser.h"

struct _CubeProjectLoader {
  CubeProjectLoaderError error;
  CubeProject *project;
};

static CubeProgram *decode_program(JsonValue *program_object) {
  if (json_value_get_type(program_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  const char *name = json_value_get_string_member(program_object, "name", "");
  StringArray *sources =
      json_value_get_string_array_member(program_object, "sources");
  if (sources == NULL) {
    sources = string_array_new();
  }
  StringArray *libraries =
      json_value_get_string_array_member(program_object, "libraries");
  if (libraries == NULL) {
    libraries = string_array_new();
  }

  CubeProgram *program = cube_program_new(name, sources, libraries);
  string_array_unref(sources);
  return program;
}

static CubeProgram **decode_programs(JsonValue *program_array,
                                     size_t *programs_length) {
  if (json_value_get_type(program_array) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  size_t programs_length_ = json_value_get_length(program_array);
  CubeProgram **programs = malloc(sizeof(CubeProgram *) * programs_length_);
  for (size_t i = 0; i < programs_length_; i++) {
    CubeProgram *program =
        decode_program(json_value_get_element(program_array, i));
    if (program == NULL) {
      // FIXME: free programs
      return NULL;
    }
    programs[i] = program;
  }

  *programs_length = programs_length_;
  return programs;
}

static CubeProject *decode_project(JsonParser *parser) {
  if (json_parser_get_error(parser) != JSON_PARSER_ERROR_NONE) {
    return NULL;
  }

  JsonValue *project_object = json_parser_get_json(parser);
  if (json_value_get_type(project_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  size_t programs_length;
  CubeProgram **programs = decode_programs(
      json_value_get_member(project_object, "programs"), &programs_length);
  if (programs == NULL) {
    return NULL;
  }

  CubeProject *project = cube_project_new(programs, programs_length);
  for (size_t i = 0; i < programs_length; i++) {
    cube_program_unref(programs[i]);
  }
  free(programs);

  return project;
}

static void load_project(CubeProjectLoader *self) {
  FILE *f = fopen("cube.json", "r");
  if (f == NULL) {
    self->error = CUBE_PROJECT_LOADER_ERROR_NO_PROJECT;
    return;
  }
  char data[1024];
  size_t n_read = fread(data, 1, sizeof(data) - 1, f);
  data[n_read] = '\0';
  fclose(f);
  JsonParser *parser = json_parser_new(data);
  self->project = decode_project(parser);
  json_parser_unref(parser);
  if (self->project == NULL) {
    self->error = CUBE_PROJECT_LOADER_ERROR_INVALID_PROJECT;
    return;
  }
}

CubeProjectLoader *cube_project_loader_new() {
  CubeProjectLoader *self = malloc(sizeof(CubeProjectLoader));

  self->error = CUBE_PROJECT_LOADER_ERROR_NONE;
  self->project = NULL;

  load_project(self);

  return self;
}

CubeProjectLoaderError cube_project_loader_get_error(CubeProjectLoader *self) {
  return self->error;
}

CubeProject *cube_project_loader_get_project(CubeProjectLoader *self) {
  return self->project;
}

void cube_project_loader_unref(CubeProjectLoader *self) {
  if (self->project != NULL) {
    cube_project_unref(self->project);
  }
  free(self);
}
