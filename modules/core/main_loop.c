#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "main_loop.h"

typedef struct {
  Fd *fd;
  MainLoopReadCallback read_callback;
  void *user_data;
  void (*user_data_unref)(void *);
} FdCallbacks;

struct _MainLoop {
  int ref;
  struct pollfd *fds;
  size_t fds_length;
  FdCallbacks *callbacks;
  Fd *quit_pipe_input;
  Fd *quit_pipe_output;
  bool done;
};

static void quit_cb(MainLoop *loop, void *user_data) {}

MainLoop *main_loop_new() {
  MainLoop *self = malloc(sizeof(MainLoop));
  self->ref = 1;
  self->fds = NULL;
  self->fds_length = 0;
  self->callbacks = NULL;
  self->quit_pipe_input = NULL;
  self->quit_pipe_output = NULL;
  self->done = false;
  return self;
}

MainLoop *main_loop_ref(MainLoop *self) {
  self->ref++;
  return self;
}

void main_loop_unref(MainLoop *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self->fds);
  for (size_t i = 0; i < self->fds_length; i++) {
    FdCallbacks *c = &self->callbacks[i];
    fd_unref(c->fd);
    if (c->user_data_unref) {
      c->user_data_unref(c->user_data);
    }
  }
  free(self->callbacks);
  if (self->quit_pipe_input) {
    fd_unref(self->quit_pipe_input);
  }
  if (self->quit_pipe_output) {
    fd_unref(self->quit_pipe_output);
  }
  free(self);
}

void main_loop_add_fd(MainLoop *self, Fd *fd,
                      MainLoopReadCallback read_callback, void *user_data,
                      void (*user_data_unref)(void *)) {
  self->fds_length++;
  self->fds = realloc(self->fds, sizeof(struct pollfd) * self->fds_length);
  self->callbacks =
      realloc(self->callbacks, sizeof(FdCallbacks) * self->fds_length);

  struct pollfd *f = &self->fds[self->fds_length - 1];
  f->fd = fd_get(fd);
  f->events = 0;
  if (read_callback != NULL) {
    f->events |= POLLIN;
  }
  f->revents = 0;

  FdCallbacks *c = &self->callbacks[self->fds_length - 1];
  c->fd = fd_ref(fd);
  c->read_callback = read_callback;
  c->user_data = user_data;
  c->user_data_unref = user_data_unref;
}

void main_loop_run(MainLoop *self) {
  int pipe_fds[2];
  // FIXME: Use pipe2 (gnu) or set CLOEXEC on outputs (racy).
  if (pipe(pipe_fds) == -1) {
    // FIXME: Error
    return;
  }
  self->quit_pipe_input = fd_new(pipe_fds[1]);
  self->quit_pipe_output = fd_new(pipe_fds[0]);
  main_loop_add_fd(self, self->quit_pipe_output, quit_cb, NULL, NULL);

  while (true) {
    poll(self->fds, self->fds_length, -1);
    if (self->done) {
      return;
    }

    for (size_t i = 0; i < self->fds_length; i++) {
      if (self->fds[i].revents & POLLIN) {
        self->callbacks[i].read_callback(self, self->callbacks[i].user_data);
      }
    }
  }
}

void main_loop_quit(MainLoop *self) {
  self->done = true;
  if (self->quit_pipe_input != NULL) {
    char data[1] = {0};
    write(fd_get(self->quit_pipe_input), data, 1);
  }
}
