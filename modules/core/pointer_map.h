#pragma once

typedef struct _PointerMap PointerMap;

#include <stdbool.h>

PointerMap *pointer_map_new(int (*key_compare_function)(void *, void *),
                            void *(*key_ref_function)(void *),
                            void (*key_unref_function)(void *),
                            void *(*value_ref_function)(void *),
                            void (*value_unref_function)(void *));

size_t pointer_map_get_length(PointerMap *self);

void pointer_map_insert(PointerMap *self, void *key, void *value);

void pointer_map_insert_take(PointerMap *self, void *key, void *value);

/// Returns the element with [key] from the map or `NULL` if no element with
/// this key.
void *pointer_map_lookup(PointerMap *self, void *key);

/// Remove element with [key] from the map. Returns true if there was an element
/// with this key.
bool pointer_map_remove(PointerMap *self, void *key);

/// Remove element with [key] from the map. Returns the element that had this
/// key or `NULL` if none. Ownership of the element is returned.
void *pointer_map_remove_take(PointerMap *self, void *key);

PointerMap *pointer_map_ref(PointerMap *self);

void pointer_map_unref(PointerMap *self);
