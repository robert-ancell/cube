#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include "string_array.h"

// FIXME: Naming

size_t string_get_length(const char *string);

// FIXME: Replace strcmp usage
bool string_matches(const char *string, const char *string2);

bool string_has_prefix(const char *string, const char *prefix);

bool string_has_suffix(const char *string, const char *suffix);

// FIXME: Replace strdup usage
char *string_copy(const char *string);

char *string_get_prefix(const char *string, size_t end);

char *string_get_suffix(const char *string, size_t start);

char *string_slice(const char *string, ssize_t start, ssize_t end);

StringArray *string_split(const char *string, const char *divider);
