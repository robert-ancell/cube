#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_parser.h"

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

static bool decode_project(JsonValue *project) {
  if (json_value_get_type(project) != JSON_VALUE_TYPE_OBJECT) {
    return false;
  }

  JsonValue *programs = json_value_get_member(project, "programs");
  if (json_value_get_type(programs) != JSON_VALUE_TYPE_ARRAY) {
    return false;
  }

  size_t programs_length = json_value_get_length(programs);
  for (size_t i = 0; i < programs_length; i++) {
    JsonValue *program = json_value_get_element(programs, i);
    if (json_value_get_type(program) != JSON_VALUE_TYPE_OBJECT) {
      return false;
    }

    const char *name = get_string_member(program, "name");
    size_t sources_length;
    const char **sources =
        get_string_array_member(program, "sources", &sources_length);

    printf("%s:\n", name);
    for (size_t j = 0; j < sources_length; j++) {
      printf(" - %s\n", sources[j]);
    }
  }

  return true;
}

static bool load_project() {
  FILE *f = fopen("cube.json", "r");
  if (f == NULL) {
    return false;
  }
  char data[1024];
  size_t n_read = fread(data, 1, sizeof(data) - 1, f);
  data[n_read] = '\0';
  fclose(f);
  JsonParser *parser = json_parser_new(data);
  if (json_parser_get_error(parser) != JSON_PARSER_ERROR_NONE) {
    return false;
  }
  bool result = decode_project(json_parser_get_json(parser));
  json_parser_free(parser);

  return result;
}

static int do_build() {
  load_project();

  return 0;
}

static int do_test() {
  load_project();

  return 0;
}

static int do_format() {
  load_project();

  return 0;
}

static int do_clean() {
  load_project();
  return 0;
}

static int do_help(int argc, char **argv) {
  fprintf(stderr,
          "Cube is a tool for building C projects.\n"
          "\n"
          "Usage: cube <command> [<options>...]\n"
          "\n"
          "Commands:\n"
          " build  Build project\n"
          " test   Run tests\n"
          " format Reformat code\n"
          " clean  Delete build artifacts\n"
          " help   Show command help\n"
          "\n"
          "For more information on a command, run 'cube help <command>'\n");
  return 0;
}

int main(int argc, char **argv) {
  const char *command = argc > 1 ? argv[1] : "help";
  int command_argc = argc - 2;
  char **command_argv = argv + 2;

  if (strcmp(command, "build") == 0) {
    return do_build();
  } else if (strcmp(command, "test") == 0) {
    return do_test();
  } else if (strcmp(command, "format") == 0) {
    return do_format();
  } else if (strcmp(command, "clean") == 0) {
    return do_clean();
  } else if (strcmp(command, "help") == 0) {
    return do_help(command_argc, command_argv);
  } else {
    fprintf(stderr, "Unknown command \"%s\", see 'cube help'\n", command);
    return 1;
  }

  return 0;
}
