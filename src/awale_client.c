#include "src/awale_server.h"
#include "src/log.h"
#include "src/socket.h"
#include "src/utils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

bool game_client(GameClient *game_client, char *server_addr, int port) {
  in_addr_t addr;
  if (inet_pton(AF_INET, server_addr, &addr) < 1) {
    LOG_ERROR("invalid IPv4 address: %s", server_addr);
    return false;
  }

  Socket sock;
  if (!client_connect(addr, port, &sock))
    return false;

  game_client->socket = sock;
  char buf[64];
  socket_to_str(&sock, buf, sizeof buf);
  LOG_DEBUG("Connected client to %s", buf);
  return true;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: awale_client server_addr port\n");
    exit(0);
  }

  int port;
  if (!parse_int(argv[2], &port, 0, 65535)) {
    printf("Port %s not in the range of 0 - 65535\n", argv[2]);
    exit(0);
  }

  GameClient client;

  if (!game_client(&client, argv[1], port)) {
    printf("Cannot establish connection, exiting\n");
    exit(0);
  }
}
