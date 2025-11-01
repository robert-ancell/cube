#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "ipv4_address.h"

struct _IPv4Address {
  int ref;
  uint32_t address;
};

static IPv4Address *ipv4_address_new_raw(uint32_t address) {
  IPv4Address *self = malloc(sizeof(IPv4Address));

  self->ref = 1;
  self->address = address;

  return self;
}

IPv4Address *ipv4_address_new(const char *address) {
  struct in_addr a;
  inet_pton(AF_INET, address, &a);
  return ipv4_address_new_raw(ntohl(a.s_addr));
}

IPv4Address *ipv4_address_new_loopback() {
  return ipv4_address_new_raw(INADDR_LOOPBACK);
}

IPv4Address *ipv4_address_new_any() { return ipv4_address_new_raw(INADDR_ANY); }

IPv4Address *ipv4_address_new_broadcast() {
  return ipv4_address_new_raw(INADDR_BROADCAST);
}

uint32_t ipv4_address_get_address(IPv4Address *self) { return self->address; }

IPv4Address *ipv4_address_ref(IPv4Address *self) {
  self->ref++;
  return self;
}

void ipv4_address_unref(IPv4Address *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self);
}
