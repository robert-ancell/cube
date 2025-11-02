#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "ipv4_address.h"

struct _Ipv4Address {
  int ref;
  uint32_t address;
};

static Ipv4Address *ipv4_address_new_raw(uint32_t address) {
  Ipv4Address *self = malloc(sizeof(Ipv4Address));

  self->ref = 1;
  self->address = address;

  return self;
}

Ipv4Address *ipv4_address_new(const char *address) {
  struct in_addr a;
  inet_pton(AF_INET, address, &a);
  return ipv4_address_new_raw(ntohl(a.s_addr));
}

Ipv4Address *ipv4_address_new_loopback() {
  return ipv4_address_new_raw(INADDR_LOOPBACK);
}

Ipv4Address *ipv4_address_new_any() { return ipv4_address_new_raw(INADDR_ANY); }

Ipv4Address *ipv4_address_new_broadcast() {
  return ipv4_address_new_raw(INADDR_BROADCAST);
}

uint32_t ipv4_address_get_address(Ipv4Address *self) { return self->address; }

Ipv4Address *ipv4_address_ref(Ipv4Address *self) {
  self->ref++;
  return self;
}

void ipv4_address_unref(Ipv4Address *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self);
}
