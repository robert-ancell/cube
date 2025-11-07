#pragma once

typedef struct _Uri Uri;

#include <stddef.h>
#include <stdint.h>

Uri *uri_new(const char *scheme, const char *user_info, const char *host,
             uint16_t port, const char *path, const char *query,
             const char *fragment);

Uri *uri_new_from_string(const char *uri);

Uri *uri_ref(Uri *self);

void uri_unref(Uri *self);
