#include "src/awale_server.h"
#include "src/commands/command.h"
#include "src/log.h"
#include "src/socket.h"
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void *game_loop(void *args);
static void server_interact(int fd);

typedef struct {
  GameServer *server;
  size_t slot;
} GameLoopArgs;

int next_free_client_slot(GameServer *game_server) {
  pthread_mutex_lock(&game_server->clients_lock);
  int slot = -1;
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (game_server->clients[i].socket.fd == -1) {
      slot = (int)i;
      break;
    }
  }
  if (slot == -1 && game_server->n_connections < MAX_CLIENTS) {
    slot = (int)game_server->n_connections;
    game_server->n_connections++;
  }
  pthread_mutex_unlock(&game_server->clients_lock);
  return slot;
}

void disconnect_client(GameServer *game_server, size_t slot) {
  pthread_mutex_lock(&game_server->clients_lock);
  game_server->clients[slot].socket.fd = -1;
  pthread_mutex_unlock(&game_server->clients_lock);
}

bool game_server(GameServer *game_server, int port) {
  Socket sock;
  if (!server_socket(htonl(INADDR_ANY), port, &sock)) {
    return false;
  }
  game_server->socket = sock;
  game_server->running = false;
  game_server->n_connections = 0;
  pthread_mutex_init(&game_server->clients_lock, NULL);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    game_server->clients[i].socket.fd = -1;
  }
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
    int slot = next_free_client_slot(game_server);
    if (slot < 0) {
      socket_close(client.fd);
      LOG_DEBUG("Server full, rejecting connection");
      continue;
    }
    game_server->clients[slot] = (GameClient){client};

    GameLoopArgs *args = malloc(sizeof *args);
    args->server = game_server;
    args->slot = (size_t)slot;
    pthread_t tid;
    pthread_create(&tid, NULL, game_loop, args);
    LOG_DEBUG("Started new thread %lu", tid);
    pthread_detach(tid);
  }
}

static void *game_loop(void *args) {
  GameLoopArgs *ga = args;
  GameServer *server = ga->server;
  size_t slot = ga->slot;
  char buf[64];
  socket_to_str(&server->socket, buf, 64);
  LOG_INFO("Start interacting with %s", buf);
  server_interact(server->clients[slot].socket.fd);
  socket_close(server->clients[slot].socket.fd);
  disconnect_client(server, slot);
  free(ga);
  LOG_INFO("Ending session with %s", buf);
  return NULL;
}

static void server_interact(int fd) {
  while (true) {
    char buf[MAX_INPUT_LEN];
    if (!socket_read_str(fd, buf, MAX_INPUT_LEN)) {
      LOG_DEBUG("failed to read from socket... disconnecting");
      return;
    }
    char *result = exec_cmd(buf);
    char send_buf[MAX_SEND_LENGTH + 1];
    size_t res_len = strlen(result);
    if (res_len >= MAX_SEND_LENGTH) {
      LOG_ERROR("Response lenth %zu longer then max send", res_len);
      char *msg = "Internal server error\n";
      socket_send(fd, msg, strlen(msg));
      continue;
    }
    memcpy(send_buf, result, res_len);
    send_buf[res_len] = '\n';
    send_buf[res_len + 1] = '\0';
    if (!socket_send(fd, send_buf, res_len + 1)) {
      LOG_ERROR("Could not send response %s", send_buf);
    }
  }
}

int main(void) {
  GameServer server;
  if (game_server(&server, 8080)) {
    start(&server);
  }
}
