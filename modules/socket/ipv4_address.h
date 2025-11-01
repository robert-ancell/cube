#pragma once

typedef struct _IPv4Address IPv4Address;

#include <stdint.h>
#include <sys/types.h>

#include "ipv4_address.h"

IPv4Address *ipv4_address_new(const char *address);

IPv4Address *ipv4_address_new_loopback();

IPv4Address *ipv4_address_new_any();

IPv4Address *ipv4_address_new_broadcast();

uint32_t ipv4_address_get_address(IPv4Address *self);

IPv4Address *ipv4_address_ref(IPv4Address *self);

void ipv4_address_unref(IPv4Address *self);
