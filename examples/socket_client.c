#include <stdio.h>

#include "ipv4_socket.h"

int main(int argc, char **argv) {
  Ipv4Socket *socket = ipv4_socket_new_tcp();
  Ipv4Address *address = ipv4_address_new_loopback();
  ipv4_socket_connect(socket, address, 1234);
  ipv4_address_unref(address);

  uint8_t buffer[1024];
  ssize_t n_read = ipv4_socket_receive(socket, buffer, sizeof(buffer) - 1);
  if (n_read < 0) {
    return -1;
  }
  buffer[n_read] = '\0';
  printf("Got response: \"%s\"\n", buffer);

  return 0;
}
