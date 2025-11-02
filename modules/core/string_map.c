#include <assert.h>
#include <stdlib.h>

#include <cube/string.h>

#include "pointer_map.h"
#include "string_map.h"

struct _StringMap {
  int ref;
  PointerMap *map;
};

static int key_compare(void *key1, void *key2) {
  const char *k1 = key1;
  const char *k2 = key2;

  // Simple binary compare
  size_t i = 0;
  while (true) {
    if (k1[i] == '\0') {
      if (k2[i] == '\0') {
        return 0;
      }
      // k1 shorter than k2
      return -1;
    } else if (k2[i] == '\0') {
      if (k1[i] == '\0') {
        return 0;
      }
      // k1 longer than k2
      return 1;
    }

    int d = k1[i] - k2[i];
    if (d != 0) {
      return d;
    }
  }
}

static void *key_ref(void *key) { return string_copy(key); }

static void key_unref(void *key) { free(key); }

StringMap *string_map_new(void *(*value_ref_function)(void *),
                          void (*value_unref_function)(void *)) {
  StringMap *self = malloc(sizeof(StringMap));

  self->ref = 1;
  self->map = pointer_map_new(key_compare, key_ref, key_unref,
                              value_ref_function, value_unref_function);

  return self;
}

size_t string_map_get_length(StringMap *self) {
  return pointer_map_get_length(self->map);
}

void string_map_insert(StringMap *self, const char *key, void *value) {
  pointer_map_insert(self->map, (void *)key, value);
}

void string_map_insert_take(StringMap *self, const char *key, void *value) {
  pointer_map_insert_take(self->map, (void *)key, value);
}

void *string_map_lookup(StringMap *self, const char *key) {
  return pointer_map_lookup(self->map, (void *)key);
}

bool string_map_remove(StringMap *self, const char *key) {
  return pointer_map_remove(self->map, (void *)key);
}

void *string_map_remove_take(StringMap *self, const char *key) {
  return pointer_map_remove_take(self->map, (void *)key);
}

StringMap *string_map_ref(StringMap *self) {
  self->ref++;
  return self;
}

void string_map_unref(StringMap *self) {
  if (--self->ref != 0) {
    return;
  }

  pointer_map_unref(self->map);
  free(self);
}
