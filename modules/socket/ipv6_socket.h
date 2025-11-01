#pragma once

typedef struct _IPv6Socket IPv6Socket;

#include <stdint.h>
#include <sys/types.h>

#include "ipv6_address.h"

IPv6Socket *ipv6_socket_new_tcp();

IPv6Socket *ipv6_socket_new_udp();

void ipv6_socket_set_blocking(IPv6Socket *self, bool blocking);

void ipv6_socket_bind(IPv6Socket *self, IPv6Address *address, uint16_t port);

void ipv6_socket_connect(IPv6Socket *self, IPv6Address *address, uint16_t port);

void ipv6_socket_send(IPv6Socket *self, const uint8_t *data,
                      size_t data_length);

ssize_t ipv6_socket_receive(IPv6Socket *self, uint8_t *buffer,
                            size_t buffer_length);

void ipv6_socket_listen(IPv6Socket *self);

IPv6Socket *ipv6_socket_accept(IPv6Socket *self);

IPv6Socket *ipv6_socket_ref(IPv6Socket *self);

void ipv6_socket_unref(IPv6Socket *self);
