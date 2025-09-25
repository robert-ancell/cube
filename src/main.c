#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "cube_command_runner.h"
#include "cube_project_loader.h"
#include "path_functions.h"
#include "string_builder.h"
#include "string_functions.h"
#include "utf8.h"

static int print_no_project_error() {
  fprintf(stderr, "No Cube project found, see `cube help create`.\n");
  return 1;
}

static int print_invalid_command_args(const char *command) {
  fprintf(stderr, "Invalid arguments, see `cube help %s`.\n", command);
  return 1;
}

static CubeProject *load_project_from_path(const char *path) {
  CubeProjectLoader *loader = cube_project_loader_new(path);
  if (cube_project_loader_get_error(loader) != CUBE_PROJECT_LOADER_ERROR_NONE) {
    return NULL;
  }

  CubeProject *project =
      cube_project_ref(cube_project_loader_get_project(loader));
  cube_project_loader_unref(loader);
  return project;
}

static CubeProject *load_project() { return load_project_from_path("."); }

static int create_project(const char *name, const char *project_path,
                          const char *source_path, const char *main_path) {
  // FIXME: Handle errors
  mkdir(name, 0777);
  mkdir(source_path, 0777);

  FILE *f = fopen(project_path, "w");
  if (f == NULL) {
    fprintf(stderr, "Unable to create project file in %s\n", project_path);
    return 1;
  }
  fprintf(f,
          "{\n"
          "  \"programs\": [\n"
          "    {\n"
          "      \"name\": \"%s\",\n"
          "      \"sources\": [\n"
          "        \"src/main.c\"\n"
          "      ]\n"
          "    }\n"
          "  ]\n"
          "}\n",
          name);
  fclose(f);

  f = fopen(main_path, "w");
  if (f == NULL) {
    fprintf(stderr, "Unable to create source file in %s\n", main_path);
    return 1;
  }
  fprintf(f, "#include <stdio.h>\n"
             "\n"
             "int main(int argc, char **argv) {\n"
             "  printf(\"Hello world!\\n\");\n"
             "  return 0;\n"
             "}\n");
  fclose(f);

  printf("Created project \"%s\"\n"
         "\n"
         "To build, run:\n"
         "$ cd %s\n"
         "$ cube build\n",
         name, name);
  return 0;
}

static int do_create(int argc, char **argv) {
  if (argc != 1) {
    return print_invalid_command_args("create");
  }
  const char *name = argv[0];

  CubeProject *project = load_project_from_path(name);
  if (project != NULL) {
    fprintf(stderr, "Not creating project \"%s\" - project already exists\n",
            name);
    cube_project_unref(project);
    return 1;
  }

  char *project_path = string_printf("%s/cube.json", name);
  char *source_path = string_printf("%s/src", name);
  char *main_path = string_printf("%s/src/main.c", name);
  int result = create_project(name, project_path, source_path, main_path);
  free(project_path);
  free(source_path);
  free(main_path);

  return result;
}

static void add_mkdir_command(CubeCommandArray *commands, const char *path) {
  StringArray *inputs = string_array_new();

  StringArray *args = string_array_new();
  // string_array_append(args, "echo");
  string_array_append(args, "mkdir");
  string_array_append(args, "-p");
  string_array_append(args, path);

  StringArray *outputs = string_array_new();
  string_array_append(outputs, path);

  char *label = string_printf("Making directory %s", path);
  cube_command_array_append_take(
      commands, cube_command_new(inputs, args, outputs, label));

  string_array_unref(inputs);
  string_array_unref(args);
  string_array_unref(outputs);
  free(label);
}

static char *get_compile_output(const char *build_dir, const char *input_path) {
  assert(string_has_suffix(input_path, ".c"));
  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, build_dir);
  string_builder_append(builder, "/");
  string_builder_append(builder, input_path);
  char *output_path = string_builder_take_string(builder);
  output_path[string_get_length(output_path) - 1] = 'o';
  string_builder_unref(builder);
  return output_path;
}

static void add_depends(StringArray *inputs, const char *build_dir,
                        const char *source) {
  char *deps_path = get_compile_output(build_dir, source);
  deps_path[string_get_length(deps_path) - 1] = 'd';

  // FIXME: Replace with file_get_contents or mmap
  FILE *f = fopen(deps_path, "r");
  if (f == NULL) {
    string_array_append(inputs, source);
    return;
  }

  // FIXME: Length and error
  char deps[1024];
  size_t n_read = fread(deps, 1, 1024 - 1, f);
  deps[n_read] = '\0';
  fclose(f);
  free(deps_path);

  size_t offset = 0;

  // Skip output
  while (true) {
    if (deps[offset] == '\0') {
      return;
    }
    if (deps[offset] == ':') {
      offset++;
      break;
    } else {
      offset++;
    }
  }

  // List of inputs.
  while (true) {
    // Skip whitespace
    while (true) {
      if (deps[offset] == ' ') {
        offset++;
      } else if (deps[offset] == '\\' && deps[offset + 1] == '\n') {
        offset += 2;
      } else if (deps[offset] == '\n' || deps[offset] == '\0') {
        return;
      } else {
        break;
      }
    }
    StringBuilder *builder = string_builder_new();
    while (true) {
      if (deps[offset] == '\0' || deps[offset] == ' ' || deps[offset] == '\n' ||
          (deps[offset] == '\\' && deps[offset] == '\n')) {
        break;
      }
      // FIXME: Handle invalid codepoint
      size_t codepoint_length;
      uint32_t codepoint =
          utf8_read_codepoint(deps + offset, &codepoint_length);
      string_builder_append_codepoint(builder, codepoint);
      offset += codepoint_length;
    }
    if (string_builder_get_length(builder) > 0) {
      string_array_append_take(inputs, string_builder_take_string(builder));
    }
    string_builder_unref(builder);
  }
}

static void add_compile_command(CubeCommandArray *commands,
                                const char *build_dir, const char *source) {
  char *output_path = get_compile_output(build_dir, source);

  StringArray *inputs = string_array_new();
  string_array_append(inputs, build_dir);
  add_depends(inputs, build_dir, source);

  StringArray *args = string_array_new();
  // string_array_append(args, "echo");
  string_array_append(args, "gcc");
  string_array_append(args, "-MMD");
  string_array_append(args, "-g");
  string_array_append(args, "-Wall");
  string_array_append(args, "-c");
  string_array_append(args, source);
  string_array_append(args, "-o");
  string_array_append(args, output_path);

  StringArray *outputs = string_array_new();
  string_array_append(outputs, output_path);

  char *label = string_printf("Compiling %s", source);
  cube_command_array_append_take(
      commands, cube_command_new(inputs, args, outputs, label));

  free(output_path);
  string_array_unref(inputs);
  string_array_unref(args);
  string_array_unref(outputs);
  free(label);
}

static void runner_command_started(CubeCommandRunner *runner,
                                   CubeCommand *command, void *user_data) {
  fprintf(stderr, "%s\n", cube_command_get_label(command));
}

static CubeCommandRunnerCallbacks runner_callbacks = {
    .command_started = runner_command_started};

static int do_build() {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  char *build_dir = string_copy(".cube/build");

  CubeProgramArray *programs = cube_project_get_programs(project);
  CubeCommandArray *commands = cube_command_array_new();

  StringArray *output_dirs = string_array_new();
  size_t programs_length = cube_program_array_get_length(programs);
  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program = cube_program_array_get_element(programs, i);
    StringArray *sources = cube_program_get_sources(program);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      char *output_path = get_compile_output(build_dir, source);
      char *output_dir = path_get_directory(output_path);
      if (!string_array_contains(output_dirs, output_dir)) {
        string_array_append(output_dirs, output_dir);
        add_mkdir_command(commands, output_dir);
      }
      free(output_path);
      free(output_dir);
    }
  }

  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program = cube_program_array_get_element(programs, i);
    const char *binary_name = cube_program_get_name(program);

    StringArray *sources = cube_program_get_sources(program);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      add_compile_command(commands, build_dir, source);
    }
    StringArray *inputs = string_array_new();
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      string_array_append_take(inputs, get_compile_output(build_dir, source));
    }

    StringArray *args = string_array_new();
    // string_array_append(args, "echo");
    string_array_append(args, "gcc");
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      string_array_append_take(args, get_compile_output(build_dir, source));
    }
    StringArray *libraries = cube_program_get_libraries(program);
    size_t libraries_length = string_array_get_length(libraries);
    for (size_t j = 0; j < libraries_length; j++) {
      const char *library = string_array_get_element(libraries, j);
      char *arg = string_printf("-l%s", library);
      string_array_append(args, arg);
      free(arg);
    }
    string_array_append(args, "-o");
    string_array_append(args, binary_name);

    StringArray *outputs = string_array_new();
    string_array_append(outputs, binary_name);

    char *label = string_printf("Linking %s", cube_program_get_name(program));
    cube_command_array_append_take(
        commands, cube_command_new(inputs, args, outputs, label));

    string_array_unref(inputs);
    string_array_unref(args);
    string_array_unref(outputs);
    free(label);
  }

  CubeCommandRunner *runner =
      cube_command_runner_new(commands, &runner_callbacks, NULL);
  cube_command_array_unref(commands);
  cube_command_runner_run(runner);

  if (cube_command_runner_get_error(runner) == CUBE_COMMAND_RUNNER_ERROR_NONE) {
    for (size_t i = 0; i < programs_length; i++) {
      CubeProgram *program = cube_program_array_get_element(programs, i);
      fprintf(stderr, "Build complete, run with `./%s`\n",
              cube_program_get_name(program));
    }
  }

  cube_project_unref(project);
  cube_command_runner_unref(runner);

  return 1;
}

static int do_test() {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: test\n");

  cube_project_unref(project);

  return 1;
}

static int do_document() {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: document\n");

  cube_project_unref(project);

  return 1;
}

static int do_format() {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: format\n");

  cube_project_unref(project);

  return 1;
}

static int do_clean() {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: clean\n");

  cube_project_unref(project);

  return 1;
}

static void print_command_help(const char *command_and_args,
                               const char *description) {
  fprintf(stderr,
          "Usage:\n"
          "  cube %s\n"
          "\n"
          "%s\n",
          command_and_args, description);
}

static int do_help(int argc, char **argv) {
  if (argc == 0) {
    fprintf(stderr,
            "Cube is a tool for building C projects.\n"
            "\n"
            "Usage: cube <command> [<options>...]\n"
            "\n"
            "Commands:\n"
            " create    Create project\n"
            " build     Build project\n"
            " test      Run tests\n"
            " document  Update project documentation\n"
            " format    Reformat code\n"
            " clean     Delete build artifacts\n"
            " help      Show command help\n"
            "\n"
            "For more information on a command, run 'cube help <command>'\n");
    return 0;
  }

  if (argc > 1) {
    return print_invalid_command_args("help");
  }

  const char *command = argv[0];
  if (string_matches(command, "create")) {
    print_command_help("create <project>", "Create a new Cube project.");
  } else if (string_matches(command, "build")) {
    print_command_help("build", "Build all artifacts for this project.");
  } else if (string_matches(command, "test")) {
    print_command_help("test", "Build and run project tests.");
  } else if (string_matches(command, "document")) {
    print_command_help("document", "Update project documentation.");
  } else if (string_matches(command, "format")) {
    print_command_help("format", "Update source code to standard style.");
  } else if (string_matches(command, "clean")) {
    print_command_help("clean", "Delete all built artifacts.");
  } else if (string_matches(command, "help")) {
    print_command_help("help [<command>]",
                       "Show help information about each Cube command.");
  } else {
    fprintf(stderr, "Unknown command \"%s\", see `cube help`.\n", command);
    return 1;
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    return do_help(0, NULL);
  }

  const char *command = argv[1];
  int command_argc = argc - 2;
  char **command_argv = argv + 2;

  if (string_matches(command, "create")) {
    return do_create(command_argc, command_argv);
  } else if (string_matches(command, "build")) {
    return do_build();
  } else if (string_matches(command, "test")) {
    return do_test();
  } else if (string_matches(command, "document")) {
    return do_document();
  } else if (string_matches(command, "format")) {
    return do_format();
  } else if (string_matches(command, "clean")) {
    return do_clean();
  } else if (string_matches(command, "help")) {
    return do_help(command_argc, command_argv);
  } else {
    fprintf(stderr, "Unknown command \"%s\", see 'cube help'\n", command);
    return 1;
  }

  return 0;
}
