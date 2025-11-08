#pragma once

typedef struct _Uri Uri;

#include <stddef.h>
#include <stdint.h>

Uri *uri_new(const char *scheme, const char *user_info, const char *host,
             uint16_t port, const char *path, const char *query,
             const char *fragment);

Uri *uri_new_from_string(const char *uri);

const char *uri_get_scheme(Uri *self);

const char *uri_get_user_info(Uri *self);

const char *uri_get_host(Uri *self);

uint16_t uri_get_port(Uri *self);

const char *uri_get_path(Uri *self);

const char *uri_get_query(Uri *self);

const char *uri_get_fragment(Uri *self);

Uri *uri_ref(Uri *self);

void uri_unref(Uri *self);
