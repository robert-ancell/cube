#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "unix_socket.h"

struct _UnixSocket {
  int ref;
  int fd;
};

static void set_address(struct sockaddr_un *a, const char *path) {
  a->sun_family = AF_UNIX;
  size_t i = 0;
  while (path[i] != '\0' && i < sizeof(a->sun_path) - 1) {
    a->sun_path[i] = path[i];
    i++;
  }
  a->sun_path[i] = '\0';
}

static UnixSocket *unix_socket_new_for_fd(int fd) {
  UnixSocket *self = malloc(sizeof(UnixSocket));

  self->ref = 1;
  self->fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);

  return self;
}

UnixSocket *unix_socket_new() {
  int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  return unix_socket_new_for_fd(fd);
}

void unix_socket_bind(UnixSocket *self, const char *path) {
  struct sockaddr_un a;
  set_address(&a, path);
  bind(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void unix_socket_connect(UnixSocket *self, const char *path) {
  struct sockaddr_un a;
  set_address(&a, path);
  connect(self->fd, (struct sockaddr *)&a, sizeof(a));
}

UnixSocket *unix_socket_accept(UnixSocket *self) {
  struct sockaddr_un a;
  socklen_t a_length = sizeof(a);
  int fd = accept(self->fd, (struct sockaddr *)&a, &a_length);
  // FIXME: Set non-blocking
  return unix_socket_new_for_fd(fd);
}

UnixSocket *unix_socket_ref(UnixSocket *self) {
  self->ref++;
  return self;
}

void unix_socket_unref(UnixSocket *self) {
  if (--self->ref != 0) {
    return;
  }

  shutdown(self->fd, SHUT_RDWR);
  free(self);
}
