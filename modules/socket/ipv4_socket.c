#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "ipv4_socket.h"

struct _Ipv4Socket {
  int ref;
  int fd;
};

static void set_address(struct sockaddr_in *a, Ipv4Address *address,
                        uint16_t port) {
  a->sin_family = AF_INET;
  a->sin_port = htons(port);
  a->sin_addr.s_addr = htonl(ipv4_address_get_address(address));
}

static Ipv4Socket *ipv4_socket_new_for_fd(int fd) {
  Ipv4Socket *self = malloc(sizeof(Ipv4Socket));

  self->ref = 1;
  self->fd = fd;

  return self;
}

Ipv4Socket *ipv4_socket_new_tcp() {
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  return ipv4_socket_new_for_fd(fd);
}

Ipv4Socket *ipv4_socket_new_udp() {
  int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  return ipv4_socket_new_for_fd(fd);
}

void ipv4_socket_set_blocking(Ipv4Socket *self, bool blocking) {
  // FIXME
}

void ipv4_socket_bind(Ipv4Socket *self, Ipv4Address *address, uint16_t port) {
  struct sockaddr_in a;
  set_address(&a, address, port);
  bind(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv4_socket_connect(Ipv4Socket *self, Ipv4Address *address,
                         uint16_t port) {
  struct sockaddr_in a;
  set_address(&a, address, port);
  connect(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv4_socket_send(Ipv4Socket *self, const uint8_t *data,
                      size_t data_length) {
  send(self->fd, data, data_length, 0);
}

ssize_t ipv4_socket_receive(Ipv4Socket *self, uint8_t *buffer,
                            size_t buffer_length) {
  return recv(self->fd, buffer, buffer_length, 0);
}

void ipv4_socket_listen(Ipv4Socket *self) { listen(self->fd, 20); }

Ipv4Socket *ipv4_socket_accept(Ipv4Socket *self) {
  struct sockaddr_in a;
  socklen_t a_length = sizeof(a);
  int fd = accept(self->fd, (struct sockaddr *)&a, &a_length);
  return ipv4_socket_new_for_fd(fd);
}

Ipv4Socket *ipv4_socket_ref(Ipv4Socket *self) {
  self->ref++;
  return self;
}

void ipv4_socket_unref(Ipv4Socket *self) {
  if (--self->ref != 0) {
    return;
  }

  shutdown(self->fd, SHUT_RDWR);
  free(self);
}
