#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Read the UTF-8 next codepoint from [text].
/// [length] is set to the number of bytes in this codepoint (1-4).
/// If [text] does not contain a valid codepoint 0 is returned and [length] set
/// to 0.
uint32_t utf8_read_codepoint(const char *text, size_t *length);
