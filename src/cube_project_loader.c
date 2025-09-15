#include <stdio.h>
#include <stdlib.h>

#include "cube_project_loader.h"
#include "json_parser.h"

struct _CubeProjectLoader {
  CubeProjectLoaderError error;
  CubeProject *project;
};

static const char *get_string_member(JsonValue *object, const char *name) {
  JsonValue *member = json_value_get_member(object, name);
  if (member == NULL || json_value_get_type(member) != JSON_VALUE_TYPE_STRING) {
    return NULL;
  }

  return json_value_get_string(member);
}

static const char **get_string_array_member(JsonValue *object, const char *name,
                                            size_t *length) {
  JsonValue *member = json_value_get_member(object, name);
  if (member == NULL || json_value_get_type(member) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  size_t member_length = json_value_get_length(member);
  const char **array = malloc(sizeof(char *) * member_length);
  for (size_t i = 0; i < member_length; i++) {
    JsonValue *element = json_value_get_element(member, i);
    if (json_value_get_type(element) != JSON_VALUE_TYPE_STRING) {
      free(array);
      return NULL;
    }
    array[i] = json_value_get_string(element);
  }

  *length = member_length;
  return array;
}

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

    const char *name = get_string_member(program, "name");
    size_t sources_length;
    const char **sources =
        get_string_array_member(program, "sources", &sources_length);

    programs[i] = cube_program_new(name, sources, sources_length);
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
