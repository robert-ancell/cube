#pragma once

typedef struct _IPv4Socket IPv4Socket;

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "ipv4_address.h"

IPv4Socket *ipv4_socket_new_tcp();

IPv4Socket *ipv4_socket_new_udp();

void ipv4_socket_set_blocking(IPv4Socket *self, bool blocking);

void ipv4_socket_bind(IPv4Socket *self, IPv4Address *address, uint16_t port);

void ipv4_socket_connect(IPv4Socket *self, IPv4Address *address, uint16_t port);

void ipv4_socket_send(IPv4Socket *self, const uint8_t *data,
                      size_t data_length);

ssize_t ipv4_socket_receive(IPv4Socket *self, uint8_t *buffer,
                            size_t buffer_length);

void ipv4_socket_listen(IPv4Socket *self);

IPv4Socket *ipv4_socket_accept(IPv4Socket *self);

IPv4Socket *ipv4_socket_ref(IPv4Socket *self);

void ipv4_socket_unref(IPv4Socket *self);
