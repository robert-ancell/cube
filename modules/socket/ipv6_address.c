#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "ipv6_address.h"

struct _Ipv6Address {
  int ref;
  uint8_t address[16];
};

static Ipv6Address *ipv6_address_new_raw(const uint8_t *address) {
  Ipv6Address *self = malloc(sizeof(Ipv6Address));

  self->ref = 1;
  for (size_t i = 0; i < 16; i++) {
    self->address[i] = address[i];
  }

  return self;
}

Ipv6Address *ipv6_address_new(const char *address) {
  struct in6_addr a;
  inet_pton(AF_INET6, address, &a);
  return ipv6_address_new_raw(a.s6_addr);
}

const uint8_t *ipv6_address_get_address(Ipv6Address *self) {
  return self->address;
}

Ipv6Address *ipv6_address_ref(Ipv6Address *self) {
  self->ref++;
  return self;
}

void ipv6_address_unref(Ipv6Address *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self);
}
