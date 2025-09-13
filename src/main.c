#include <stdio.h>
#include <string.h>

static int do_build() { return 0; }

static int do_clean() { return 0; }

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
