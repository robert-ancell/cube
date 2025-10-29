#pragma once

typedef struct _PointerArray PointerArray;

PointerArray *pointer_array_new(void *(*ref_function)(void *),
                                void (*unref_function)(void *));

void pointer_array_append(PointerArray *self, void *element);

void pointer_array_append_take(PointerArray *self, void *element);

size_t pointer_array_get_length(PointerArray *self);

void *pointer_array_get_element(PointerArray *self, size_t i);

PointerArray *pointer_array_ref(PointerArray *self);

void pointer_array_unref(PointerArray *self);
