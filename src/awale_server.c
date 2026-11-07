#include "src/awale_server.h"
#include "src/awale/awale.h"
#include "src/log.h"
#include "src/socket.h"
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void *game_loop(void *args);

bool game_server(GameServer *game_server, int port) {
  Socket sock;
  if (!server_socket(htonl(INADDR_ANY), port, &sock)) {
    return false;
  }
  game_server->socket = sock;
  game_server->running = false;
  char buf[64];
  socket_to_str(&game_server->socket, buf, sizeof buf);
  LOG_DEBUG("Created game server on %s", buf);
  return true;
}

void start(GameServer *game_server) {
  game_server->running = true;
  LOG_INFO("Started Awale game server!");
  while (game_server->running) {
    Socket client;
    LOG_DEBUG("Awaiting connection...");
    if (!accept_connection(game_server->socket.fd, &client)) {
      continue;
    }
    char buf[64];
    socket_to_str(&client, buf, sizeof buf);
    LOG_INFO("Acceped new connection from client on %s", buf);
    pthread_t tid;
    pthread_create(&tid, NULL, game_loop, ((void *)&client));
    pthread_detach(tid);
  }
}

bool server_interact(int fd, char *prompt, char *resp, int resp_len) {
  while (true) {
    if (!socket_read(fd, prompt, resp_len)) {
      continue;
    }
  }
}

static bool get_name(int fd, char *name, int length) {
  char *prompt = "Hello! - Please enter your name:";
  return server_interact(fd, prompt, name, length);
}

static void *game_loop(void *args) {
  Socket *client = (Socket *)args;

  char name[PLAYER_NAME_MAX_LENGTH + 1];
  if (get_name(client->fd, name, sizeof(name)))
    printf("Received %s\nBye!\n", name);

  return NULL;
}

int main(void) {
  GameServer server;
  if (game_server(&server, 8080)) {
    start(&server);
  }
}
