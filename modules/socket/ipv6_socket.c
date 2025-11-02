#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "ipv6_socket.h"

struct _Ipv6Socket {
  int ref;
  int fd;
};

static void set_address(struct sockaddr_in6 *a, Ipv6Address *address,
                        uint16_t port) {
  a->sin6_family = AF_INET6;
  a->sin6_port = htons(port);
  a->sin6_flowinfo = 0;
  const uint8_t *addr = ipv6_address_get_address(address);
  for (size_t i = 0; i < 16; i++) {
    a->sin6_addr.s6_addr[i] = addr[i];
  }
}

static Ipv6Socket *ipv6_socket_new_for_fd(int fd) {
  Ipv6Socket *self = malloc(sizeof(Ipv6Socket));

  self->ref = 1;
  self->fd = fd;

  return self;
}

Ipv6Socket *ipv6_socket_new_tcp() {
  int fd = socket(AF_INET6, SOCK_STREAM | SOCK_CLOEXEC, 0);
  return ipv6_socket_new_for_fd(fd);
}

Ipv6Socket *ipv6_socket_new_udp() {
  int fd = socket(AF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  return ipv6_socket_new_for_fd(fd);
}

void ipv6_socket_set_blocking(Ipv6Socket *self, bool blocking) {
  // FIXME
}

void ipv6_socket_bind(Ipv6Socket *self, Ipv6Address *address, uint16_t port) {
  struct sockaddr_in6 a;
  set_address(&a, address, port);
  bind(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv6_socket_connect(Ipv6Socket *self, Ipv6Address *address,
                         uint16_t port) {
  struct sockaddr_in6 a;
  set_address(&a, address, port);
  connect(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv6_socket_send(Ipv6Socket *self, const uint8_t *data,
                      size_t data_length) {
  send(self->fd, data, data_length, 0);
}

ssize_t ipv6_socket_receive(Ipv6Socket *self, uint8_t *buffer,
                            size_t buffer_length) {
  return recv(self->fd, buffer, buffer_length, 0);
}

void ipv6_socket_listen(Ipv6Socket *self) { listen(self->fd, 20); }

Ipv6Socket *ipv6_socket_accept(Ipv6Socket *self) {
  struct sockaddr_in6 a;
  socklen_t a_length = sizeof(a);
  int fd = accept(self->fd, (struct sockaddr *)&a, &a_length);
  return ipv6_socket_new_for_fd(fd);
}

Ipv6Socket *ipv6_socket_ref(Ipv6Socket *self) {
  self->ref++;
  return self;
}

void ipv6_socket_unref(Ipv6Socket *self) {
  if (--self->ref != 0) {
    return;
  }

  shutdown(self->fd, SHUT_RDWR);
  free(self);
}
