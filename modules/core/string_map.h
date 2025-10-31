#pragma once

typedef struct _StringMap StringMap;

#include <stdbool.h>

StringMap *string_map_new(void *(*value_ref_function)(void *),
                          void (*value_unref_function)(void *));

size_t string_map_get_length(StringMap *self);

void string_map_insert(StringMap *self, const char *key, void *value);

void string_map_insert_take(StringMap *self, const char *key, void *value);

/// Returns the element with [key] from the map or `NULL` if no element with
/// this key.
void *string_map_lookup(StringMap *self, const char *key);

/// Remove element with [key] from the map. Returns true if there was an element
/// with this key.
bool string_map_remove(StringMap *self, const char *key);

/// Remove element with [key] from the map. Returns the element that had this
/// key or `NULL` if none. Ownership of the element is returned.
void *string_map_remove_take(StringMap *self, const char *key);

StringMap *string_map_ref(StringMap *self);

void string_map_unref(StringMap *self);
