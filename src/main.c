#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "json_parser.h"

static bool load_project() {
  JsonParser *parser = json_parser_new("{\"one\": true, \"two\": false}");
  JsonValue *value = json_parser_get_json(parser);
  printf("%s\n", json_value_to_string(value));
  json_parser_free(parser);

  return true;
}

static int do_build() {
  load_project();

  return 0;
}

static int do_clean() {
  load_project();
  return 0;
}

static int do_help() {
  fprintf(stderr, "Cube is a tool for building C projects.\n"
                  "\n"
                  "Usage: cube <command> [<options>...]\n"
                  "\n"
                  "Commands:\n"
                  " build  Build current project\n"
                  " clean  Delete build artifacts\n"
                  " help   Show command help\n");
  return 0;
}

int main(int argc, char **argv) {
  const char *command = argc > 1 ? argv[1] : "help";

  if (strcmp(command, "build") == 0) {
    return do_build();
  } else if (strcmp(command, "clean") == 0) {
    return do_clean();
  } else if (strcmp(command, "help") == 0) {
    return do_help();
  } else {
    fprintf(stderr, "Unknown command \"%s\", see 'cube help'\n", command);
    return 1;
  }

  return 0;
}
