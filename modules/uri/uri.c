#include <stdlib.h>
#include <string.h>

#include <cube/string.h>

#include "uri.h"

struct _Uri {
  int ref;
  char *scheme;
  char *user_info;
  char *host;
  uint16_t port;
  char *path;
  char *query;
  char *fragment;
};

static Uri *uri_new_take(char *scheme, char *user_info, char *host,
                         uint16_t port, char *path, char *query,
                         char *fragment) {
  Uri *self = malloc(sizeof(Uri));

  self->ref = 1;
  self->scheme = scheme;
  self->user_info = user_info;
  self->host = host;
  self->port = port;
  self->path = path;
  self->query = query;
  self->fragment = fragment;

  return self;
}

Uri *uri_new(const char *scheme, const char *user_info, const char *host,
             uint16_t port, const char *path, const char *query,
             const char *fragment) {
  return uri_new_take(strdup(scheme), user_info ? strdup(user_info) : NULL,
                      strdup(host), port, path ? strdup(path) : NULL,
                      query ? strdup(query) : NULL,
                      fragment ? strdup(fragment) : NULL);
}

Uri *uri_new_from_string(const char *uri) {
  size_t scheme_start = 0, scheme_end = 0;
  while (uri[scheme_end] != '\0' && uri[scheme_end] != ':') {
    scheme_end++;
  }
  if (uri[scheme_end] != ':') {
    return NULL; // Missing scheme terminator
  }
  char *scheme = string_slice(uri, scheme_start, scheme_end);

  char *user_info = NULL;
  char *host = NULL;
  uint16_t port = 0;
  char *path = NULL;
  char *query = NULL;
  char *fragment = NULL;
  size_t hier_part_start = scheme_end + 1, hier_part_end;
  if (string_has_prefix(uri + hier_part_start, "//")) {
    size_t authority_start = hier_part_start + 2,
           authority_end = authority_start;
    while (uri[authority_end] != '\0' && uri[authority_end] != '/' &&
           uri[authority_end] != '?' && uri[authority_end] != '#') {
      authority_end++;
    }

    size_t userinfo_start = authority_start, userinfo_end = userinfo_start;
    while (userinfo_end != authority_end && uri[userinfo_end] != '@') {
      userinfo_end++;
    }
    size_t host_start;
    if (userinfo_end != authority_end) {
      user_info = string_slice(uri, userinfo_start, userinfo_end);
      host_start = userinfo_end + 1;
    } else {
      host_start = authority_start;
    }

    size_t host_end = host_start;
    if (uri[host_start] == '[') {
      while (uri[host_end] != '\0' && uri[host_end] != ']') {
        host_end++;
      }
      if (uri[host_end] != ']') {
        return NULL; // Unterminated literal IP address
      }
      host_end++;
    } else {
      while (host_end != authority_end && uri[host_end] != ':') {
        host_end++;
      }
    }
    host = string_slice(uri, host_start, host_end);

    if (uri[host_end] == ':') {
      size_t port_start = host_end + 1, port_end = authority_end;
      char *port_string = string_slice(uri, port_start, port_end);
      port = atoi(port_string);
      free(port_string);
    }
    hier_part_end = authority_end;
  } else {
    hier_part_end = hier_part_start;
  }

  size_t path_start = hier_part_end, path_end = path_start;
  while (uri[path_end] != '\0' && uri[path_end] != '?' &&
         uri[path_end] != '#') {
    path_end++;
  }
  path = string_slice(uri, path_start, path_end);

  if (uri[path_end] == '?') {
    size_t query_start = path_end + 1, query_end = query_start;
    while (uri[query_end] != '\0' && uri[query_end] != '#') {
      query_end++;
    }
    query = string_slice(uri, query_start, query_end);
    path_end = query_end;
  }

  if (uri[path_end] == '#') {
    size_t fragment_start = path_end + 1, fragment_end = fragment_start;
    while (uri[fragment_end] != '\0' && uri[fragment_end] != '#') {
      fragment_end++;
    }
    fragment = string_slice(uri, fragment_start, fragment_end);
  }

  return uri_new_take(scheme, user_info, host, port, path, query, fragment);
}

const char *uri_get_scheme(Uri *self) { return self->scheme; }

const char *uri_get_user_info(Uri *self) { return self->user_info; }

const char *uri_get_host(Uri *self) { return self->host; }

uint16_t uri_get_port(Uri *self) { return self->port; }

const char *uri_get_path(Uri *self) { return self->path; }

const char *uri_get_query(Uri *self) { return self->query; }

const char *uri_get_fragment(Uri *self) { return self->fragment; }

Uri *uri_ref(Uri *self) {
  self->ref++;
  return self;
}

void uri_unref(Uri *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->scheme);
  free(self->user_info);
  free(self->host);
  free(self->path);
  free(self->query);
  free(self->fragment);
  free(self);
}
