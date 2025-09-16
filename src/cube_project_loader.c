#include <stdio.h>
#include <stdlib.h>

#include "cube_project_loader.h"
#include "json_parser.h"

struct _CubeProjectLoader {
  CubeProjectLoaderError error;
  CubeProject *project;
};

static CubeProject *decode_project(JsonValue *project) {
  if (json_value_get_type(project) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  JsonValue *program_array = json_value_get_member(project, "programs");
  if (json_value_get_type(program_array) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  size_t programs_length = json_value_get_length(program_array);
  CubeProgram **programs = malloc(sizeof(CubeProgram *) * programs_length);
  for (size_t i = 0; i < programs_length; i++) {
    JsonValue *program = json_value_get_element(program_array, i);
    if (json_value_get_type(program) != JSON_VALUE_TYPE_OBJECT) {
      // FIXME: free programs
      return NULL;
    }

    const char *name = json_value_get_string_member(program, "name", "");
    StringArray *sources =
        json_value_get_string_array_member(program, "sources");
    if (sources == NULL) {
      sources = string_array_new();
    }

    programs[i] = cube_program_new(name, sources);
    string_array_unref(sources);
  }

  return cube_project_new(programs, programs_length);
}

static void load_project(CubeProjectLoader *self) {
  FILE *f = fopen("cube.json", "r");
  if (f == NULL) {
    return;
  }
  char data[1024];
  size_t n_read = fread(data, 1, sizeof(data) - 1, f);
  data[n_read] = '\0';
  fclose(f);
  JsonParser *parser = json_parser_new(data);
  if (json_parser_get_error(parser) != JSON_PARSER_ERROR_NONE) {
    json_parser_unref(parser);
    return;
  }
  self->project = decode_project(json_parser_get_json(parser));
  json_parser_unref(parser);
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
