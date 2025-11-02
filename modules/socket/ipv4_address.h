#pragma once

typedef struct _Ipv4Address Ipv4Address;

#include <stdint.h>
#include <sys/types.h>

#include "ipv4_address.h"

Ipv4Address *ipv4_address_new(const char *address);

Ipv4Address *ipv4_address_new_loopback();

Ipv4Address *ipv4_address_new_any();

Ipv4Address *ipv4_address_new_broadcast();

uint32_t ipv4_address_get_address(Ipv4Address *self);

Ipv4Address *ipv4_address_ref(Ipv4Address *self);

void ipv4_address_unref(Ipv4Address *self);
