#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cube_project_loader.h"

static CubeProject *load_project() {
  CubeProjectLoader *loader = cube_project_loader_new();
  if (cube_project_loader_get_error(loader) != CUBE_PROJECT_LOADER_ERROR_NONE) {
    return NULL;
  }

  CubeProject *project =
      cube_project_ref(cube_project_loader_get_project(loader));
  cube_project_loader_unref(loader);
  return project;
}

static int do_create() {
  load_project();

  return 0;
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
          " create  Create project\n"
          " build   Build project\n"
          " test    Run tests\n"
          " format  Reformat code\n"
          " clean   Delete build artifacts\n"
          " help    Show command help\n"
          "\n"
          "For more information on a command, run 'cube help <command>'\n");
  return 0;
}

int main(int argc, char **argv) {
  const char *command = argc > 1 ? argv[1] : "help";
  int command_argc = argc - 2;
  char **command_argv = argv + 2;

  if (strcmp(command, "create") == 0) {
    return do_create();
  } else if (strcmp(command, "build") == 0) {
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
