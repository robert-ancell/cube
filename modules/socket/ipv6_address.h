#pragma once

typedef struct _IPv6Address IPv6Address;

#include <stdint.h>
#include <sys/types.h>

#include "ipv6_address.h"

IPv6Address *ipv6_address_new(const char *address);

const uint8_t *ipv6_address_get_address(IPv6Address *self);

IPv6Address *ipv6_address_ref(IPv6Address *self);

void ipv6_address_unref(IPv6Address *self);
