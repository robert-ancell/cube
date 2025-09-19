#include "path_functions.h"
#include "string_functions.h"

static int get_separator_index(const char *path) {
  int separator_index = -1;
  for (size_t i = 0; path[i] != '\0'; i++) {
    if (path[i] == '/') {
      separator_index = i;
    }
  }

  return separator_index;
}

char *path_get_directory(const char *path) {
  int separator_index = get_separator_index(path);
  if (separator_index < 0) {
    return NULL;
  }
  return string_get_prefix(path, separator_index);
}

char *path_get_filename(const char *path) {
  int separator_index = get_separator_index(path);
  if (separator_index < 0) {
    return string_copy(path);
  }
  return string_get_suffix(path, separator_index);
}
