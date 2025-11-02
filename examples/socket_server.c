#include <stdio.h>
#include <string.h>

#include "ipv4_socket.h"

int main(int argc, char **argv) {
  uint16_t port = 1234;
  Ipv4Socket *socket = ipv4_socket_new_tcp();
  Ipv4Address *address = ipv4_address_new_any();
  ipv4_socket_bind(socket, address, port);
  ipv4_address_unref(address);

  ipv4_socket_listen(socket);
  printf("Listening on port %d\n", port);

  while (true) {
    Ipv4Socket *client = ipv4_socket_accept(socket);
    printf("Got connection!\n");
    const char *message = "Hello world!";
    ipv4_socket_send(client, (const uint8_t *)message, strlen(message) + 1);
    ipv4_socket_unref(client);
  }

  return 0;
}
