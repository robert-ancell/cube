#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_functions.h"

static size_t negative_index(size_t length, ssize_t index) {
  if (index >= 0) {
    return index;
  } else if (-index >= length) {
    return 0;
  } else {
    return length + index;
  }
}

static size_t match_length(const char *string, const char *string2) {
  size_t count = 0;
  while (string[count] == string2[count] && string[count] != '\0' &&
         string2[count] != '\0') {
    count++;
  }
  return count;
}

char *string_copy(const char *string) {
  size_t string_length = string_get_length(string);
  char *copy = malloc(string_length + 1);
  for (size_t i = 0; i <= string_length; i++) {
    copy[i] = string[i];
  }
  return copy;
}

char *string_printf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  char empty = '\0';
  va_list ap2;
  va_copy(ap2, ap);
  int length = vsnprintf(&empty, 1, format, ap2);
  va_end(ap2);
  char *string = malloc(sizeof(char) * (length + 1));
  vsnprintf(string, length + 1, format, ap);
  va_end(ap);

  return string;
}

size_t string_get_length(const char *string) {
  size_t string_length = 0;
  while (string[string_length] != '\0')
    string_length++;
  return string_length;
}

bool string_matches(const char *string1, const char *string2) {
  size_t count = match_length(string1, string2);
  return string1[count] == '\0' && string2[count] == '\0';
}

bool string_has_prefix(const char *string, const char *prefix) {
  size_t count = match_length(string, prefix);
  return prefix[count] == '\0';
}

bool string_has_suffix(const char *string, const char *suffix) {
  size_t string_length = string_get_length(string);
  size_t suffix_length = string_get_length(suffix);
  return string_matches(string + string_length - suffix_length, suffix);
}

char *string_get_prefix(const char *string, size_t end) {
  return string_slice(string, 0, end);
}

char *string_get_suffix(const char *string, size_t start) {
  size_t string_length = string_get_length(string);
  return string_slice(string, start, string_length);
}

char *string_slice(const char *string, ssize_t start, ssize_t end) {
  size_t string_length = string_get_length(string);

  start = negative_index(string_length, start);
  end = negative_index(string_length, end);
  if (end > string_length) {
    end = string_length;
  }
  if (start > end) {
    start = end;
  }

  size_t slice_length = end - start;
  char *slice = malloc(slice_length + 1);
  for (size_t i = 0; i < slice_length; i++) {
    slice[i] = string[start + i];
  }
  slice[slice_length] = '\0';

  return slice;
}

StringArray *string_split(const char *string, const char *divider) {
  // FIXME
  return string_array_new();
}
