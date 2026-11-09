#include "src/awale_server.h"
#include "src/commands/command.h"
#include "src/log.h"
#include "src/socket.h"
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

static void* game_loop(void* args);
static bool server_interact(int fd);

bool game_server(GameServer* game_server, int port) {
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

void start(GameServer* game_server) {
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
    if (game_server->n_connections >= MAX_CLIENTS) {
      socket_close(client.fd);
      LOG_DEBUG("Closing socket because there are %zu active connections",
                game_server->n_connections);
      continue;
    }
    game_server->clients[game_server->n_connections] = (GameClient){client};
    game_server->n_connections++;

    pthread_t tid;
    pthread_create(&tid, NULL, game_loop,
                   &game_server->clients[game_server->n_connections - 1]);
    LOG_DEBUG("Started new thread %lu", tid);
    pthread_detach(tid);
  }
}

static void* game_loop(void* args) {
  GameClient* client = (GameClient*)args;
  server_interact(client->socket.fd);
  return NULL;
}

static bool server_interact(int fd) {
  while (true) {
    char buf[MAX_INPUT_LEN];
    if (!socket_read_str(fd, buf, MAX_INPUT_LEN)) {
      LOG_ERROR("failed to read from socket");
      continue;
    }
    exec_cmd(buf);
  }
}

int main(void) {
  GameServer server;
  if (game_server(&server, 8080)) {
    start(&server);
  }
}
