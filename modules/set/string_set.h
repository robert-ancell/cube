#pragma once

typedef struct _StringSet StringSet;

#include <stdbool.h>

StringSet *string_set_new();

size_t string_set_get_length(StringSet *self);

void string_set_insert(StringSet *self, const char *value);

void string_set_insert_take(StringSet *self, const char *value);

bool string_set_contains(StringSet *self, const char *value);

/// Remove element with [value] from the set. Returns true if there was an
/// element with this value.
bool string_set_remove(StringSet *self, const char *value);

StringSet *string_set_ref(StringSet *self);

void string_set_unref(StringSet *self);
