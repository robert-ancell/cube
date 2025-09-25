#pragma once

typedef struct _StringBuilder StringBuilder;

#include <stdint.h>

StringBuilder *string_builder_new();

void string_builder_append(StringBuilder *self, const char *string);

void string_builder_append_codepoint(StringBuilder *self, uint32_t codepoint);

size_t string_builder_get_length(StringBuilder *self);

/// Get the string currently built. Owned by the builder.
const char *string_builder_get_string(StringBuilder *self);

/// Get the built string. The builder will now contain an empty string.
char *string_builder_take_string(StringBuilder *self);

StringBuilder *string_builder_ref(StringBuilder *self);

void string_builder_unref(StringBuilder *self);
