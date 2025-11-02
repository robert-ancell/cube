#pragma once

typedef struct _Ipv4Socket Ipv4Socket;

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "ipv4_address.h"

Ipv4Socket *ipv4_socket_new_tcp();

Ipv4Socket *ipv4_socket_new_udp();

void ipv4_socket_set_blocking(Ipv4Socket *self, bool blocking);

void ipv4_socket_bind(Ipv4Socket *self, Ipv4Address *address, uint16_t port);

void ipv4_socket_connect(Ipv4Socket *self, Ipv4Address *address, uint16_t port);

void ipv4_socket_send(Ipv4Socket *self, const uint8_t *data,
                      size_t data_length);

ssize_t ipv4_socket_receive(Ipv4Socket *self, uint8_t *buffer,
                            size_t buffer_length);

void ipv4_socket_listen(Ipv4Socket *self);

Ipv4Socket *ipv4_socket_accept(Ipv4Socket *self);

Ipv4Socket *ipv4_socket_ref(Ipv4Socket *self);

void ipv4_socket_unref(Ipv4Socket *self);
