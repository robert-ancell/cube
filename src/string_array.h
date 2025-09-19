#pragma once

typedef struct _StringArray StringArray;

#include <stdbool.h>

StringArray *string_array_new();

void string_array_append(StringArray *self, const char *string);

void string_array_append_take(StringArray *self, char *string);

size_t string_array_get_length(StringArray *self);

const char *string_array_get_element(StringArray *self, size_t i);

bool string_array_contains(StringArray *self, const char *string);

StringArray *string_array_ref(StringArray *self);

void string_array_unref(StringArray *self);
