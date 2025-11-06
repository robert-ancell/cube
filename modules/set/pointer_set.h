#pragma once

typedef struct _PointerSet PointerSet;

#include <stdbool.h>

PointerSet *pointer_set_new(int (*compare_function)(void *, void *),
                            void *(*ref_function)(void *),
                            void (*unref_function)(void *));

size_t pointer_set_get_length(PointerSet *self);

void pointer_set_insert(PointerSet *self, void *value);

void pointer_set_insert_take(PointerSet *self, void *value);

bool pointer_set_contains(PointerSet *self, void *value);

/// Remove element with [value] from the set. Returns true if there was an
/// element with this value.
bool pointer_set_remove(PointerSet *self, void *value);

PointerSet *pointer_set_ref(PointerSet *self);

void pointer_set_unref(PointerSet *self);
