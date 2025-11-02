#pragma once

typedef struct _Ipv6Address Ipv6Address;

#include <stdint.h>
#include <sys/types.h>

#include "ipv6_address.h"

Ipv6Address *ipv6_address_new(const char *address);

const uint8_t *ipv6_address_get_address(Ipv6Address *self);

Ipv6Address *ipv6_address_ref(Ipv6Address *self);

void ipv6_address_unref(Ipv6Address *self);
