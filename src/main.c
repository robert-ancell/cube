#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <cube/path.h>
#include <cube/string.h>
#include <cube/uri.h>

#include "cube_command_runner.h"
#include "cube_project_builder.h"
#include "cube_project_loader.h"

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

static int do_analyze(int argc, char **argv) {
  printf("FIXME: analyze\n");
  return 0;
}

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
          "  \"imports\": [\n"
          "    {\n"
          "      \"url\": \"https://github.com/robert-ancell/cube.git\",\n"
          "      \"modules\": [\n"
          "        \"json\"\n"
          "      ]\n"
          "    }\n"
          "  ],\n"
          "\n"
          "  \"programs\": [\n"
          "    {\n"
          "      \"name\": \"%s\",\n"
          "      \"modules\": [\n"
          "        \"json\"\n"
          "      ],\n"
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

  cube_project_unref(project);

  return result;
}

static void runner_command_started(CubeCommandRunner *runner,
                                   CubeCommand *command, void *user_data) {
  fprintf(stderr, "%s\n", cube_command_get_label(command));
}

static CubeCommandRunnerCallbacks runner_callbacks = {
    .command_started = runner_command_started};

static void add_directory(StringArray *directories, const char *path) {
  if (string_array_contains(directories, path)) {
    return;
  }

  char *parent = path_get_directory(path);
  if (parent != NULL) {
    add_directory(directories, parent);
  }
  free(parent);
  string_array_append(directories, path);
}

static int do_build(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("build");
  }

  CubeProjectBuilder *builder = cube_project_builder_new();
  int return_value = 0;
  if (!cube_project_builder_run(builder)) {
    switch (cube_project_builder_get_error(builder)) {
    case CUBE_PROJECT_BUILDER_ERROR_NO_PROJECT:
      return_value = print_no_project_error();
      break;
    default:
      return_value = 1;
      break;
    }
  }
  cube_project_builder_unref(builder);

  return return_value;
}

static int do_test(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("test");
  }

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: test\n");

  cube_project_unref(project);

  return 1;
}

static int do_document(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("document");
  }

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  printf("FIXME: document\n");

  cube_project_unref(project);

  return 1;
}

static bool file_exists(const char *path) {
  struct stat info = {};
  return stat(path, &info) == 0;
}

static char *get_source_header(const char *source) {
  if (!string_has_suffix(source, ".c")) {
    return NULL;
  }

  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, string_slice(source, 0, -2));
  string_builder_append(builder, ".h");
  char *header = string_builder_take_string(builder);
  string_builder_unref(builder);
  return header;
}

static void add_format_source(StringArray *args, const char *source) {
  string_array_append(args, source);
  char *header = get_source_header(source);
  if (header == NULL) {
    return;
  }
  if (!file_exists(header)) {
    free(header);
    return;
  }

  string_array_append_take(args, header);
}

static int do_format(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("format");
  }

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  StringArray *args = string_array_new();
  string_array_append(args, "clang-format");
  string_array_append(args, "-i");
  CubeProgramArray *programs = cube_project_get_programs(project);
  size_t programs_length = cube_program_array_get_length(programs);
  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program = cube_program_array_get_element(programs, i);
    StringArray *sources = cube_program_get_sources(program);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      add_format_source(args, source);
    }
  }
  CubeModuleArray *modules = cube_project_get_modules(project);
  size_t modules_length = cube_module_array_get_length(modules);
  for (size_t i = 0; i < modules_length; i++) {
    CubeModule *module = cube_module_array_get_element(modules, i);
    StringArray *sources = cube_module_get_sources(module);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      add_format_source(args, source);
    }
  }

  CubeCommandArray *commands = cube_command_array_new();
  StringArray *inputs = string_array_new();
  StringArray *outputs = string_array_new();
  cube_command_array_append_take(
      commands, cube_command_new_take(inputs, args, outputs,
                                      string_copy("Formatting sources")));

  CubeCommandRunner *runner =
      cube_command_runner_new_take(commands, &runner_callbacks, NULL);
  cube_command_runner_run(runner);

  // FIXME: Print summary of what was formatted

  cube_project_unref(project);
  cube_command_runner_unref(runner);

  return 0;
}

static int do_clean(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("clean");
  }

  // FIXME: Make --all or --imports

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  CubeCommandArray *commands = cube_command_array_new();

  StringArray *args = string_array_new();
  string_array_append(args, "rm");
  string_array_append(args, "--recursive");
  string_array_append(args, "--force");
  string_array_append(args, ".cube/build");
  cube_command_array_append_take(
      commands,
      cube_command_new_take(string_array_new(), args, string_array_new(),
                            string_copy("Removing artifacts")));

  CubeCommandRunner *runner =
      cube_command_runner_new_take(commands, &runner_callbacks, NULL);
  cube_command_runner_run(runner);

  cube_project_unref(project);
  cube_command_runner_unref(runner);

  return 1;
}

static char *get_import_dir(CubeImport *import) {
  const char *imports_dir = ".cube/imports";

  Uri *uri = uri_new_from_string(cube_import_get_url(import));
  char *dir = string_printf("%s/%s/%s%s", imports_dir, uri_get_scheme(uri),
                            uri_get_host(uri), uri_get_path(uri));
  uri_unref(uri);

  return dir;
}

static int do_update(int argc, char **argv) {
  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  CubeCommandArray *commands = cube_command_array_new();
  StringArray *directories = string_array_new();

  CubeImportArray *imports = cube_project_get_imports(project);
  size_t imports_length = cube_import_array_get_length(imports);
  for (size_t i = 0; i < imports_length; i++) {
    CubeImport *import = cube_import_array_get_element(imports, i);
    char *dir = get_import_dir(import);
    add_directory(directories, dir);

    char *git_dir = string_printf("%s/.git", dir);
    bool have_repository = file_exists(git_dir);
    free(git_dir);

    StringArray *args = string_array_new();
    if (!have_repository) {
      string_array_append(args, "git");
      string_array_append(args, "clone");
      string_array_append(args, cube_import_get_url(import));
      string_array_append(args, dir);
    } else {
      string_array_append(args, "git");
      string_array_append(args, "-C");
      string_array_append(args, dir);
      string_array_append(args, "pull");
    }

    cube_command_array_append_take(
        commands,
        cube_command_new_take(string_array_new(), args, string_array_new(),
                              string_copy("Updating imports")));
    free(dir);
  }

  size_t directories_length = string_array_get_length(directories);
  for (size_t i = 0; i < directories_length; i++) {
    const char *path = string_array_get_element(directories, i);
    mkdir(path, 0777);
  }
  string_array_unref(directories);

  CubeCommandRunner *runner =
      cube_command_runner_new_take(commands, &runner_callbacks, NULL);
  cube_command_runner_run(runner);

  cube_project_unref(project);
  cube_command_runner_unref(runner);

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
            " update    Update imports\n"
            " build     Build project\n"
            " test      Run tests\n"
            " clean     Delete build artifacts\n"
            " analyze   Analyze project\n"
            " document  Generate project documentation\n"
            " format    Reformat code\n"
            " help      Show command help\n"
            "\n"
            "For more information on a command, run 'cube help <command>'\n");
    return 0;
  }

  if (argc > 1) {
    return print_invalid_command_args("help");
  }

  const char *command = argv[0];
  if (string_matches(command, "analyze")) {
    print_command_help("analyze", "Analyze project.");
  } else if (string_matches(command, "build")) {
    print_command_help("build", "Build all artifacts for this project.");
  } else if (string_matches(command, "clean")) {
    print_command_help("clean", "Delete all built artifacts.");
  } else if (string_matches(command, "create")) {
    print_command_help("create <project>", "Create a new Cube project.");
  } else if (string_matches(command, "document")) {
    print_command_help("document", "Generate project documentation.");
  } else if (string_matches(command, "format")) {
    print_command_help("format", "Update source code to standard style.");
  } else if (string_matches(command, "help")) {
    print_command_help("help [<command>]",
                       "Show help information about each Cube command.");
  } else if (string_matches(command, "test")) {
    print_command_help("test", "Build and run project tests.");
  } else if (string_matches(command, "update")) {
    print_command_help("update", "Update imports.");
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

  if (string_matches(command, "analyze")) {
    return do_analyze(command_argc, command_argv);
  } else if (string_matches(command, "build")) {
    return do_build(command_argc, command_argv);
  } else if (string_matches(command, "clean")) {
    return do_clean(command_argc, command_argv);
  } else if (string_matches(command, "create")) {
    return do_create(command_argc, command_argv);
  } else if (string_matches(command, "document")) {
    return do_document(command_argc, command_argv);
  } else if (string_matches(command, "format")) {
    return do_format(command_argc, command_argv);
  } else if (string_matches(command, "help")) {
    return do_help(command_argc, command_argv);
  } else if (string_matches(command, "test")) {
    return do_test(command_argc, command_argv);
  } else if (string_matches(command, "update")) {
    return do_update(command_argc, command_argv);
  } else {
    fprintf(stderr, "Unknown command \"%s\", see 'cube help'\n", command);
    return 1;
  }

  return 0;
}
