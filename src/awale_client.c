#include "src/awale_server.h"
#include "src/log.h"
#include "src/socket.h"
#include "src/utils.h"
#include <arpa/inet.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_RESPONSE_LEN 1024

static bool client_interact(int fd);
static char* client_prompt(char* prompt);

bool game_client(GameClient* game_client, char* server_addr, int port) {
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
  client_interact(sock.fd);
  return true;
}

static bool client_interact(int fd) {
  while (true) {
    char* inp = client_prompt("> ");
    if (inp == NULL) {
      printf("\n");
      return true; /* EOF (Ctrl-D): quit instead of busy-looping */
    }
    size_t len = strlen(inp);
    char msg[len + 1];
    memcpy(msg, inp, len);
    msg[len] = '\0'; /* message terminator */
    socket_send(fd, msg, len + 1);
    free(inp);

    char resp[MAX_RESPONSE_LEN];
    if (!socket_read_str(fd, resp, MAX_RESPONSE_LEN)) {
      LOG_ERROR("Could not read response...");
      continue;
    }
    printf("%s\n", resp);
  }
}

static char* client_prompt(char* prompt) {
  return readline(prompt); /* NULL on EOF */
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: awale_client server_addr port\n");
    exit(0);
  }

  int port;
  if (!parse_int(argv[2], &port, 0, 65535)) {
    printf("Port %s not in the range of 0 - 65535\n", argv[2]);
    exit(0);
  }

  GameClient client = {0};

  if (!game_client(&client, argv[1], port)) {
    printf("Cannot establish connection, exiting\n");
    exit(0);
  }
}
