#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cube_command_runner.h"
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
  CubeProject *project = load_project();

  printf("FIXME\n");

  cube_project_unref(project);

  return 1;
}

static size_t string_array_length(char **array) {
  size_t length = 0;
  while (array[length] != NULL) {
    length++;
  }
  return length;
}

static CubeCommand **add_command_take(CubeCommand **commands,
                                      size_t *commands_length,
                                      CubeCommand *command) {
  (*commands_length)++;
  commands = realloc(commands, sizeof(CubeCommand *) * *commands_length);
  commands[*commands_length - 1] = command;
  return commands;
}

static int do_build() {
  CubeProject *project = load_project();

  size_t programs_length;
  CubeProgram **programs = cube_project_get_programs(project, &programs_length);
  CubeCommand **commands = NULL;
  size_t commands_length = 0;
  for (size_t i = 0; i < programs_length; i++) {
    size_t sources_length;
    char **sources = cube_program_get_sources(programs[i], &sources_length);
    for (size_t j = 0; j < sources_length; j++) {
      char *inputs[] = {sources[j], NULL};
      char *args[] = {"echo", "compile", sources[j], NULL};
      commands = add_command_take(
          commands, &commands_length,
          cube_command_new(inputs, string_array_length(inputs), args,
                           string_array_length(args), NULL, 0));
    }
    char *args[] = {"echo", "link", (char *)cube_program_get_name(programs[i]),
                    NULL};
    commands = add_command_take(
        commands, &commands_length,
        cube_command_new(sources, string_array_length(sources), args,
                         string_array_length(args), NULL, 0));
  }

  CubeCommandRunner *runner =
      cube_command_runner_new(commands, commands_length);
  cube_command_runner_run(runner);
  cube_command_runner_unref(runner);

  cube_project_unref(project);

  return 1;
}

static int do_test() {
  CubeProject *project = load_project();

  printf("FIXME\n");

  cube_project_unref(project);

  return 1;
}

static int do_format() {
  CubeProject *project = load_project();

  printf("FIXME\n");

  cube_project_unref(project);

  return 1;
}

static int do_clean() {
  CubeProject *project = load_project();

  printf("FIXME\n");

  cube_project_unref(project);

  return 1;
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
