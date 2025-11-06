#include <assert.h>
#include <stdlib.h>

#include "pointer_map.h"

struct _PointerMap {
  int ref;
  int (*key_compare_function)(void *, void *);
  void *(*key_ref_function)(void *);
  void (*key_unref_function)(void *);
  void *(*value_ref_function)(void *);
  void (*value_unref_function)(void *);
  void **keys;
  void **values;
  size_t length;
};

/// Find element with [key] in the map returning `true` if it exists and setting
/// [index].
static bool lookup(PointerMap *self, void *key, size_t *index) {
  // FIXME: Replace with a binary tree
  for (size_t i = 0; i < self->length; i++) {
    if (self->key_compare_function(self->keys[i], key) == 0) {
      *index = i;
      return true;
    }
  }

  return false;
}

PointerMap *pointer_map_new(int (*key_compare_function)(void *, void *),
                            void *(*key_ref_function)(void *),
                            void (*key_unref_function)(void *),
                            void *(*value_ref_function)(void *),
                            void (*value_unref_function)(void *)) {
  PointerMap *self = malloc(sizeof(PointerMap));

  self->ref = 1;
  self->key_function_function = key_function_function;
  self->key_ref_function = key_ref_function;
  self->key_unref_function = key_unref_function;
  self->value_ref_function = value_ref_function;
  self->value_unref_function = value_unref_function;
  self->keys = NULL;
  self->values = NULL;
  self->length = 0;

  return self;
}

size_t pointer_map_get_length(PointerMap *self) { return self->length; }

void pointer_map_insert(PointerMap *self, void *key, void *value) {
  assert(key != NULL);
  assert(value != NULL);

  pointer_map_insert_take(self, self->key_ref_function(key),
                          self->value_ref_function(value));
}

void pointer_map_insert_take(PointerMap *self, void *key, void *value) {
  assert(key != NULL);
  assert(value != NULL);

  size_t index;
  if (lookup(self, key, &index)) {
    void *old_key = self->keys[index];
    void *old_value = self->values[index];
    self->keys[index] = key;
    self->values[index] = value;
    self->key_unref_function(old_key);
    self->value_unref_function(old_value);
  } else {
    self->length++;
    self->keys = realloc(self->keys, sizeof(void *) * self->length);
    self->values = realloc(self->values, sizeof(void *) * self->length);
    self->keys[self->length - 1] = key;
    self->values[self->length - 1] = value;
  }
}

void *pointer_map_lookup(PointerMap *self, void *key) {
  size_t index;
  if (!lookup(self, key, &index)) {
    return NULL;
  }

  return self->values[index];
}

bool pointer_map_remove(PointerMap *self, void *key) {
  void *value = pointer_map_remove_take(self, key);
  if (value == NULL) {
    return false;
  }

  self->value_unref_function(value);
  return true;
}

void *pointer_map_remove_take(PointerMap *self, void *key) {
  size_t index;
  if (!lookup(self, key, &index)) {
    return NULL;
  }

  self->key_unref_function(self->keys[index]);
  void *value = self->values[index];
  for (size_t i = index; i < self->length - 1; i++) {
    self->keys[index] = self->keys[index + 1];
    self->values[index] = self->values[index + 1];
  }
  self->length--;
  self->keys = realloc(self->keys, sizeof(void *) * self->length);
  self->values = realloc(self->values, sizeof(void *) * self->length);

  return value;
}

PointerMap *pointer_map_ref(PointerMap *self) {
  self->ref++;
  return self;
}

void pointer_map_unref(PointerMap *self) {
  if (--self->ref != 0) {
    return;
  }

  for (size_t i = 0; i < self->length; i++) {
    self->key_unref_function(self->keys[i]);
    self->value_unref_function(self->values[i]);
  }
  free(self->keys);
  free(self->values);
  free(self);
}
