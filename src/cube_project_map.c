#include <assert.h>
#include <stdlib.h>

#include <cube/map.h>

#include "cube_project_map.h"

struct _CubeProjectMap {
  StringMap *map;
};

static void *value_ref(void *value) { return cube_project_ref(value); }

static void value_unref(void *value) { cube_project_unref(value); }

CubeProjectMap *cube_project_map_new() {
  CubeProjectMap *self = malloc(sizeof(CubeProjectMap));

  self->map = string_map_new(value_ref, value_unref);

  return self;
}

size_t cube_project_map_get_length(CubeProjectMap *self) {
  return string_map_get_length(self->map);
}

void cube_project_map_insert(CubeProjectMap *self, const char *key,
                             CubeProject *value) {
  string_map_insert(self->map, key, value);
}

CubeProject *cube_project_map_lookup(CubeProjectMap *self, const char *key) {
  return string_map_lookup(self->map, key);
}

void cube_project_map_unref(CubeProjectMap *self) {
  string_map_unref(self->map);
  free(self);
}
