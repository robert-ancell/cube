#pragma once

typedef struct _Ipv6Socket Ipv6Socket;

#include <stdint.h>
#include <sys/types.h>

#include "ipv6_address.h"

Ipv6Socket *ipv6_socket_new_tcp();

Ipv6Socket *ipv6_socket_new_udp();

void ipv6_socket_set_blocking(Ipv6Socket *self, bool blocking);

void ipv6_socket_bind(Ipv6Socket *self, Ipv6Address *address, uint16_t port);

void ipv6_socket_connect(Ipv6Socket *self, Ipv6Address *address, uint16_t port);

void ipv6_socket_send(Ipv6Socket *self, const uint8_t *data,
                      size_t data_length);

ssize_t ipv6_socket_receive(Ipv6Socket *self, uint8_t *buffer,
                            size_t buffer_length);

void ipv6_socket_listen(Ipv6Socket *self);

Ipv6Socket *ipv6_socket_accept(Ipv6Socket *self);

Ipv6Socket *ipv6_socket_ref(Ipv6Socket *self);

void ipv6_socket_unref(Ipv6Socket *self);
