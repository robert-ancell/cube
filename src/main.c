#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "json_parser.h"

static bool load_project() {
  FILE *f = fopen("cube.json", "r");
  char data[1024];
  size_t n_read = fread(data, 1, sizeof(data) - 1, f);
  data[n_read] = '\0';
  fclose(f);
  JsonParser *parser = json_parser_new(data);
  JsonValue *value = json_parser_get_json(parser);
  printf("%s\n", json_value_to_string(value));
  json_parser_free(parser);

  return true;
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

static int do_help() {
  fprintf(stderr, "Cube is a tool for building C projects.\n"
                  "\n"
                  "Usage: cube <command> [<options>...]\n"
                  "\n"
                  "Commands:\n"
                  " build  Build project\n"
                  " test   Run tests\n"
                  " format Reformat code\n"
                  " clean  Delete build artifacts\n"
                  " help   Show command help\n");
  return 0;
}

int main(int argc, char **argv) {
  const char *command = argc > 1 ? argv[1] : "help";

  if (strcmp(command, "build") == 0) {
    return do_build();
  } else if (strcmp(command, "test") == 0) {
    return do_test();
  } else if (strcmp(command, "format") == 0) {
    return do_format();
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
