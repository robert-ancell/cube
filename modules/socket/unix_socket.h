#pragma once

typedef struct _UnixSocket UnixSocket;

UnixSocket *unix_socket_new();

void unix_socket_bind(UnixSocket *self, const char *path);

void unix_socket_connect(UnixSocket *self, const char *path);

void unix_socket_listen(UnixSocket *self);

UnixSocket *unix_socket_accept(UnixSocket *self);

UnixSocket *unix_socket_ref(UnixSocket *self);

void unix_socket_unref(UnixSocket *self);
