#include <stdio.h>
#include <stdlib.h>

#include <cube/json.h>
#include <cube/string.h>

#include "cube_module.h"
#include "cube_project_loader.h"

struct _CubeProjectLoader {
  CubeProjectLoaderError error;
  CubeProject *project;
};

static StringArray *get_optional_string_array(JsonValue *value,
                                              const char *name) {
  StringArray *member = json_value_get_string_array_member(value, name);
  if (member == NULL) {
    member = string_array_new();
  }
  return member;
}

static CubeDefineArray *get_defines(JsonValue *object) {
  JsonValue *member = json_value_get_object_member(object, "defines");
  CubeDefineArray *defines = cube_define_array_new();
  if (member == NULL || json_value_get_type(member) != JSON_VALUE_TYPE_OBJECT) {
    return defines;
  }

  size_t defines_length = json_value_get_length(member);
  for (size_t i = 0; i < defines_length; i++) {
    const char *define_name = json_value_get_member_name(member, i);
    JsonValue *define_value = json_value_get_member_value(member, i);
    if (json_value_get_type(define_value) != JSON_VALUE_TYPE_STRING) {
      continue;
    }
    cube_define_array_append_take(
        defines,
        cube_define_new(define_name, json_value_get_string(define_value)));
  }

  return defines;
}

static CubeProgram *decode_program(JsonValue *program_object) {
  if (json_value_get_type(program_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  const char *name = json_value_get_string_member(program_object, "name", "");
  StringArray *sources = get_optional_string_array(program_object, "sources");
  StringArray *modules = get_optional_string_array(program_object, "modules");
  CubeDefineArray *defines = get_defines(program_object);
  StringArray *libraries =
      get_optional_string_array(program_object, "libraries");

  return cube_program_new_take(name, sources, modules, defines, libraries);
}

static CubeProgramArray *decode_programs(JsonValue *program_array) {
  if (program_array == NULL ||
      json_value_get_type(program_array) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  size_t programs_length = json_value_get_length(program_array);
  CubeProgramArray *programs = cube_program_array_new();
  for (size_t i = 0; i < programs_length; i++) {
    CubeProgram *program =
        decode_program(json_value_get_element(program_array, i));
    if (program == NULL) {
      cube_program_array_unref(programs);
      return NULL;
    }
    cube_program_array_append_take(programs, program);
  }

  return programs;
}

static CubeImport *decode_import(JsonValue *import_object) {
  if (json_value_get_type(import_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  const char *url = json_value_get_string_member(import_object, "url", "");
  StringArray *modules = get_optional_string_array(import_object, "modules");

  return cube_import_new_take(url, modules);
}

static CubeImportArray *decode_imports(JsonValue *import_array) {
  if (import_array == NULL ||
      json_value_get_type(import_array) != JSON_VALUE_TYPE_ARRAY) {
    return NULL;
  }

  size_t imports_length = json_value_get_length(import_array);
  CubeImportArray *imports = cube_import_array_new();
  for (size_t i = 0; i < imports_length; i++) {
    CubeImport *import = decode_import(json_value_get_element(import_array, i));
    if (import == NULL) {
      cube_import_array_unref(imports);
      return NULL;
    }
    cube_import_array_append_take(imports, import);
  }

  return imports;
}

static CubeModule *decode_module(const char *name, JsonValue *module_object) {
  if (json_value_get_type(module_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  StringArray *sources = get_optional_string_array(module_object, "sources");
  StringArray *modules = get_optional_string_array(module_object, "modules");
  StringArray *libraries =
      get_optional_string_array(module_object, "libraries");
  StringArray *include_directories =
      get_optional_string_array(module_object, "include-directories");

  return cube_module_new_take(name, sources, modules, libraries,
                              include_directories);
}

static CubeModuleArray *decode_modules(JsonValue *module_array) {
  if (module_array == NULL ||
      json_value_get_type(module_array) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  size_t modules_length = json_value_get_length(module_array);
  CubeModuleArray *modules = cube_module_array_new();
  for (size_t i = 0; i < modules_length; i++) {
    CubeModule *module =
        decode_module(json_value_get_member_name(module_array, i),
                      json_value_get_member_value(module_array, i));
    if (module == NULL) {
      cube_module_array_unref(modules);
      return NULL;
    }
    cube_module_array_append_take(modules, module);
  }

  return modules;
}

static CubeProject *decode_project(JsonParser *parser) {
  if (json_parser_get_error(parser) != JSON_PARSER_ERROR_NONE) {
    return NULL;
  }

  JsonValue *project_object = json_parser_get_json(parser);
  if (json_value_get_type(project_object) != JSON_VALUE_TYPE_OBJECT) {
    return NULL;
  }

  CubeProgramArray *programs =
      decode_programs(json_value_get_member(project_object, "programs"));
  if (programs == NULL) {
    programs = cube_program_array_new();
  }

  CubeImportArray *imports =
      decode_imports(json_value_get_member(project_object, "imports"));
  if (imports == NULL) {
    imports = cube_import_array_new();
  }

  CubeModuleArray *modules =
      decode_modules(json_value_get_member(project_object, "modules"));
  if (modules == NULL) {
    modules = cube_module_array_new();
  }

  return cube_project_new_take(programs, imports, modules);
}

static void load_project(CubeProjectLoader *self, const char *path) {
  char *project_path = string_printf("%s/cube.json", path);
  FILE *f = fopen(project_path, "r");
  free(project_path);
  if (f == NULL) {
    self->error = CUBE_PROJECT_LOADER_ERROR_NO_PROJECT;
    return;
  }
  char data[4096];
  size_t n_read = fread(data, 1, sizeof(data) - 1, f);
  data[n_read] = '\0';
  fclose(f);
  JsonParser *parser = json_parser_new(data);
  self->project = decode_project(parser);
  json_parser_unref(parser);
  if (self->project == NULL) {
    self->error = CUBE_PROJECT_LOADER_ERROR_INVALID_PROJECT;
    return;
  }
}

CubeProjectLoader *cube_project_loader_new(const char *path) {
  CubeProjectLoader *self = malloc(sizeof(CubeProjectLoader));

  self->error = CUBE_PROJECT_LOADER_ERROR_NONE;
  self->project = NULL;

  load_project(self, path);

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
