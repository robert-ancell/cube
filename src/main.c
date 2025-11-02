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

static char *get_compile_output(const char *build_dir, const char *input_path) {
  assert(string_has_suffix(input_path, ".c"));
  StringBuilder *builder = string_builder_new();
  string_builder_append(builder, build_dir);
  string_builder_append(builder, "/");
  char *base = string_slice(input_path, 0, -2);
  string_builder_append(builder, base);
  free(base);
  string_builder_append(builder, ".bc");
  char *output_path = string_builder_take_string(builder);
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

/// `true` if [t2] is later than [t1].
static bool is_later(struct timespec *t1, struct timespec *t2) {
  return t2->tv_sec > t1->tv_sec ||
         (t2->tv_sec == t1->tv_sec && t2->tv_nsec > t1->tv_nsec);
}

// FIXME: Will fail if inputs are modified during build. Either check after
// build or use file hashes or similar.
static bool needs_building(CubeCommand *command) {
  StringArray *inputs = cube_command_get_inputs(command);
  StringArray *outputs = cube_command_get_outputs(command);

  // FIXME: Need to check if inputs are about to be rebuilt
  struct timespec youngest_input_time = {};
  size_t inputs_length = string_array_get_length(inputs);
  for (size_t i = 0; i < inputs_length; i++) {
    const char *input = string_array_get_element(inputs, i);
    struct stat info;
    if (stat(input, &info) != 0) {
      // Input missing - build to get error.
      return true;
    }
    if (is_later(&youngest_input_time, &info.st_mtim)) {
      youngest_input_time = info.st_mtim;
    }
  }

  size_t outputs_length = string_array_get_length(outputs);
  for (size_t i = 0; i < outputs_length; i++) {
    const char *output = string_array_get_element(outputs, i);
    struct stat info;
    if (stat(output, &info) != 0) {
      // Output missing - build.
      return true;
    }
    if (is_later(&info.st_mtim, &youngest_input_time)) {
      // There is an input newer than this output - rebuild.
      return true;
    }
  }

  // All outputs newer than inputs, no build required.
  return false;
}

static void add_compile_command(CubeCommandArray *commands,
                                StringArray *include_directories,
                                const char *build_dir, const char *source) {
  char *output_path = get_compile_output(build_dir, source);

  StringArray *inputs = string_array_new();
  string_array_append(inputs, build_dir);
  add_depends(inputs, build_dir, source);

  StringArray *args = string_array_new();
  string_array_append(args, "clang");
  string_array_append(args, "-std=gnu23");
  string_array_append(args, "-O3");
  string_array_append(args, "-MMD");
  string_array_append(args, "-g");
  string_array_append(args, "-Wall");
  string_array_append(args, "-c");
  string_array_append(args, "-flto");
  size_t include_directories_length =
      string_array_get_length(include_directories);
  for (size_t i = 0; i < include_directories_length; i++) {
    const char *import = string_array_get_element(include_directories, i);
    string_array_append_take(args, string_printf("-I%s", import));
  }
  string_array_append(args, source);
  string_array_append(args, "-o");
  string_array_append(args, output_path);

  StringArray *outputs = string_array_new();
  string_array_append(outputs, output_path);

  CubeCommand *command = cube_command_new_take(
      inputs, args, outputs, string_printf("Compiling %s", source));
  if (needs_building(command)) {
    cube_command_array_append_take(commands, command);
  } else {
    cube_command_unref(command);
  }

  free(output_path);
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

static void add_compile_directory(StringArray *directories,
                                  const char *build_dir, const char *source) {
  char *output_path = get_compile_output(build_dir, source);
  char *output_dir = path_get_directory(output_path);
  add_directory(directories, output_dir);
  free(output_path);
  free(output_dir);
}

static void add_module_modules(CubeProject *project, CubeModule *module,
                               StringArray *all_module_names,
                               CubeModuleArray *all_modules);

static void add_module(CubeProject *project, const char *module_name,
                       StringArray *all_module_names,
                       CubeModuleArray *all_modules) {
  if (string_array_contains(all_module_names, module_name)) {
    return;
  }

  CubeModule *module = cube_project_get_module(project, module_name);
  if (module == NULL) {
    return;
  }

  string_array_append(all_module_names, module_name);
  cube_module_array_append(all_modules, module);
  add_module_modules(project, module, all_module_names, all_modules);
}

static void add_modules(CubeProject *project, StringArray *module_names,
                        StringArray *all_module_names,
                        CubeModuleArray *all_modules) {
  size_t module_names_length = string_array_get_length(module_names);
  for (size_t i = 0; i < module_names_length; i++) {
    const char *module_name = string_array_get_element(module_names, i);
    add_module(project, module_name, all_module_names, all_modules);
  }
}

// Recursively add module depdencies to [all_module_names]
static void add_module_modules(CubeProject *project, CubeModule *module,
                               StringArray *all_module_names,
                               CubeModuleArray *all_modules) {
  add_modules(project, cube_module_get_modules(module), all_module_names,
              all_modules);
}

// Get all the modules this program uses (including dependencies).
static CubeModuleArray *get_program_modules(CubeProject *project,
                                            CubeProgram *program) {
  StringArray *all_module_names = string_array_new();
  CubeModuleArray *all_modules = cube_module_array_new();
  add_modules(project, cube_program_get_modules(program), all_module_names,
              all_modules);
  string_array_unref(all_module_names);
  return all_modules;
}

// Get all the modules this module uses (including dependencies)
static CubeModuleArray *get_module_modules(CubeProject *project,
                                           CubeModule *module) {
  StringArray *all_module_names = string_array_new();
  CubeModuleArray *all_modules = cube_module_array_new();
  add_module_modules(project, module, all_module_names, all_modules);
  string_array_unref(all_module_names);
  return all_modules;
}

// Get all the include directories required for the given modules
static StringArray *get_module_include_directories(CubeModuleArray *modules) {
  StringArray *include_directories = string_array_new();
  size_t modules_length = cube_module_array_get_length(modules);
  for (size_t i = 0; i < modules_length; i++) {
    CubeModule *module = cube_module_array_get_element(modules, i);
    StringArray *module_include_directories =
        cube_module_get_include_directories(module);
    size_t module_include_directories_length =
        string_array_get_length(module_include_directories);
    for (size_t l = 0; l < module_include_directories_length; l++) {
      string_array_append(
          include_directories,
          string_array_get_element(module_include_directories, l));
    }
  }

  return include_directories;
}

static int do_build(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("build");
  }

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  const char *build_dir = ".cube/build";

  CubeProgramArray *programs = cube_project_get_programs(project);
  CubeCommandArray *commands = cube_command_array_new();

  StringArray *output_dirs = string_array_new();
  size_t programs_length = cube_program_array_get_length(programs);
  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program = cube_program_array_get_element(programs, i);

    CubeModuleArray *modules = get_program_modules(project, program);
    size_t modules_length = cube_module_array_get_length(modules);
    for (size_t j = 0; j < modules_length; j++) {
      CubeModule *module = cube_module_array_get_element(modules, j);
      StringArray *module_sources = cube_module_get_sources(module);
      size_t module_sources_length = string_array_get_length(module_sources);
      for (size_t k = 0; k < module_sources_length; k++) {
        const char *module_source = string_array_get_element(module_sources, k);
        add_compile_directory(output_dirs, build_dir, module_source);
      }
    }

    StringArray *sources = cube_program_get_sources(program);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      add_compile_directory(output_dirs, build_dir, source);
    }

    cube_module_array_unref(modules);
  }
  size_t output_dirs_length = string_array_get_length(output_dirs);
  for (size_t i = 0; i < output_dirs_length; i++) {
    const char *path = string_array_get_element(output_dirs, i);
    mkdir(path, 0777);
  }
  string_array_unref(output_dirs);

  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program = cube_program_array_get_element(programs, i);
    const char *binary_name = cube_program_get_name(program);

    StringArray *all_sources = string_array_new();
    StringArray *inputs = string_array_new();

    CubeModuleArray *modules = get_program_modules(project, program);
    size_t modules_length = cube_module_array_get_length(modules);
    StringArray *include_directories = string_array_new();
    for (size_t j = 0; j < modules_length; j++) {
      CubeModule *module = cube_module_array_get_element(modules, j);

      CubeModuleArray *module_modules = get_module_modules(project, module);
      StringArray *module_source_include_directories =
          get_module_include_directories(module_modules);
      cube_module_array_unref(module_modules);

      StringArray *module_sources = cube_module_get_sources(module);
      size_t module_sources_length = string_array_get_length(module_sources);
      for (size_t k = 0; k < module_sources_length; k++) {
        const char *module_source = string_array_get_element(module_sources, k);

        add_compile_command(commands, module_source_include_directories,
                            build_dir, module_source);

        string_array_append_take(inputs,
                                 get_compile_output(build_dir, module_source));

        string_array_append(all_sources, module_source);
      }
      string_array_unref(module_source_include_directories);

      StringArray *module_include_directories =
          cube_module_get_include_directories(module);
      size_t module_include_directories_length =
          string_array_get_length(module_include_directories);
      for (size_t k = 0; k < module_include_directories_length; k++) {
        string_array_append(
            include_directories,
            string_array_get_element(module_include_directories, k));
      }
    }

    StringArray *sources = cube_program_get_sources(program);
    size_t sources_length = string_array_get_length(sources);
    for (size_t j = 0; j < sources_length; j++) {
      const char *source = string_array_get_element(sources, j);
      add_compile_command(commands, include_directories, build_dir, source);
      string_array_append_take(inputs, get_compile_output(build_dir, source));
      string_array_append(all_sources, source);
    }

    StringArray *args = string_array_new();
    string_array_append(args, "clang");
    string_array_append(args, "-std=gnu23");
    string_array_append(args, "-O3");
    size_t all_sources_length = string_array_get_length(all_sources);
    for (size_t j = 0; j < all_sources_length; j++) {
      const char *source = string_array_get_element(all_sources, j);
      string_array_append_take(args, get_compile_output(build_dir, source));
    }
    CubeDefineArray *defines = cube_program_get_defines(program);
    size_t defines_length = cube_define_array_get_length(defines);
    for (size_t j = 0; j < defines_length; j++) {
      CubeDefine *define = cube_define_array_get_element(defines, j);
      char *arg = string_printf("-D%s=%s", cube_define_get_name(define),
                                cube_define_get_value(define));
      string_array_append(args, arg);
      free(arg);
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

    CubeCommand *command = cube_command_new_take(
        inputs, args, outputs,
        string_printf("Linking %s", cube_program_get_name(program)));
    if (needs_building(command)) {
      cube_command_array_append_take(commands, command);
    } else {
      cube_command_unref(command);
    }

    cube_module_array_unref(modules);
  }

  CubeCommandRunner *runner =
      cube_command_runner_new(commands, &runner_callbacks, NULL);
  cube_command_array_unref(commands);
  cube_command_runner_run(runner);

  int return_value = 0;
  if (cube_command_runner_get_error(runner) == CUBE_COMMAND_RUNNER_ERROR_NONE) {
    for (size_t i = 0; i < programs_length; i++) {
      CubeProgram *program = cube_program_array_get_element(programs, i);
      fprintf(stderr, "Build complete, run with `./%s`\n",
              cube_program_get_name(program));
    }
  } else {
    return_value = 1;
  }

  cube_project_unref(project);
  cube_command_runner_unref(runner);

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
      string_array_append(args, source);
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
      string_array_append(args, source);
    }
  }

  CubeCommandArray *commands = cube_command_array_new();
  StringArray *inputs = string_array_new();
  StringArray *outputs = string_array_new();
  cube_command_array_append_take(
      commands, cube_command_new_take(inputs, args, outputs,
                                      string_copy("Formatting sources")));

  CubeCommandRunner *runner =
      cube_command_runner_new(commands, &runner_callbacks, NULL);
  cube_command_array_unref(commands);
  cube_command_runner_run(runner);

  // FIXME: Print summary of what was formatted

  cube_project_unref(project);

  return 0;
}

static int do_clean(int argc, char **argv) {
  if (argc != 0) {
    return print_invalid_command_args("clean");
  }

  CubeProject *project = load_project();
  if (project == NULL) {
    return print_no_project_error();
  }

  CubeCommandArray *commands = cube_command_array_new();

  StringArray *args = string_array_new();
  string_array_append(args, "rm");
  string_array_append(args, "--recursive");
  string_array_append(args, "--force");
  string_array_append(args, ".cube");
  cube_command_array_append_take(
      commands,
      cube_command_new_take(string_array_new(), args, string_array_new(),
                            string_copy("Removing artifacts")));

  CubeCommandRunner *runner =
      cube_command_runner_new(commands, &runner_callbacks, NULL);
  cube_command_array_unref(commands);
  cube_command_runner_run(runner);

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
    return do_build(command_argc, command_argv);
  } else if (string_matches(command, "test")) {
    return do_test(command_argc, command_argv);
  } else if (string_matches(command, "document")) {
    return do_document(command_argc, command_argv);
  } else if (string_matches(command, "format")) {
    return do_format(command_argc, command_argv);
  } else if (string_matches(command, "clean")) {
    return do_clean(command_argc, command_argv);
  } else if (string_matches(command, "help")) {
    return do_help(command_argc, command_argv);
  } else {
    fprintf(stderr, "Unknown command \"%s\", see 'cube help'\n", command);
    return 1;
  }

  return 0;
}
