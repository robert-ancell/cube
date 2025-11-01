#include <stdlib.h>
#include <unistd.h>

#include "fd.h"

struct _Fd {
  int ref;
  int fd;
};

Fd *fd_new(int fd) {
  Fd *self = malloc(sizeof(Fd));
  self->ref = 1;
  self->fd = fd;
  return self;
}

Fd *fd_ref(Fd *self) {
  self->ref++;
  return self;
}

void fd_unref(Fd *self) {
  if (--self->ref != 0) {
    return;
  }

  if (self->fd >= 0) {
    close(self->fd);
  }
  free(self);
}

int fd_get(Fd *self) { return self->fd; }

int fd_take(Fd *self) {
  int fd = self->fd;
  self->fd = -1;
  return fd;
}
