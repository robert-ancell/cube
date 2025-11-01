#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "ipv4_socket.h"

struct _IPv4Socket {
  int ref;
  int fd;
};

static void set_address(struct sockaddr_in *a, IPv4Address *address,
                        uint16_t port) {
  a->sin_family = AF_INET;
  a->sin_port = htons(port);
  a->sin_addr.s_addr = htonl(ipv4_address_get_address(address));
}

static IPv4Socket *ipv4_socket_new_for_fd(int fd) {
  IPv4Socket *self = malloc(sizeof(IPv4Socket));

  self->ref = 1;
  self->fd = fd;

  return self;
}

IPv4Socket *ipv4_socket_new_tcp() {
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  return ipv4_socket_new_for_fd(fd);
}

IPv4Socket *ipv4_socket_new_udp() {
  int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  return ipv4_socket_new_for_fd(fd);
}

void ipv4_socket_set_blocking(IPv4Socket *self, bool blocking) {
  // FIXME
}

void ipv4_socket_bind(IPv4Socket *self, IPv4Address *address, uint16_t port) {
  struct sockaddr_in a;
  set_address(&a, address, port);
  bind(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv4_socket_connect(IPv4Socket *self, IPv4Address *address,
                         uint16_t port) {
  struct sockaddr_in a;
  set_address(&a, address, port);
  connect(self->fd, (struct sockaddr *)&a, sizeof(a));
}

void ipv4_socket_send(IPv4Socket *self, const uint8_t *data,
                      size_t data_length) {
  send(self->fd, data, data_length, 0);
}

ssize_t ipv4_socket_receive(IPv4Socket *self, uint8_t *buffer,
                            size_t buffer_length) {
  return recv(self->fd, buffer, buffer_length, 0);
}

void ipv4_socket_listen(IPv4Socket *self) { listen(self->fd, 20); }

IPv4Socket *ipv4_socket_accept(IPv4Socket *self) {
  struct sockaddr_in a;
  socklen_t a_length = sizeof(a);
  int fd = accept(self->fd, (struct sockaddr *)&a, &a_length);
  return ipv4_socket_new_for_fd(fd);
}

IPv4Socket *ipv4_socket_ref(IPv4Socket *self) {
  self->ref++;
  return self;
}

void ipv4_socket_unref(IPv4Socket *self) {
  if (--self->ref != 0) {
    return;
  }

  shutdown(self->fd, SHUT_RDWR);
  free(self);
}
