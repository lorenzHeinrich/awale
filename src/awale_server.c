#include "src/awale_server.h"
#include "src/commands/command.h"
#include "src/log.h"
#include "src/socket.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  GameServer *server;
  size_t slot;
} GameLoopArgs;

bool game_server(GameServer *game_server, int port) {
  Socket sock;
  if (!server_socket(htonl(INADDR_ANY), port, &sock)) {
    return false;
  }
  game_server->socket = sock;
  game_server->running = false;
  clients_init(&game_server->registry);
  char buf[64];
  socket_to_str(&game_server->socket, buf, sizeof buf);
  LOG_DEBUG("Created game server on %s", buf);
  return true;
}

static void server_interact(int fd, GameServer *server, size_t client_slot) {
  while (true) {
    char buf[MAX_INPUT_LEN];
    if (!socket_read_str(fd, buf, MAX_INPUT_LEN)) {
      LOG_DEBUG("failed to read from socket... disconnecting");
      return;
    }
    char *result = exec_cmd(buf, &server->registry, client_slot);
    char send_buf[MAX_SEND_LENGTH + 1];
    size_t res_len = strlen(result);
    if (res_len >= MAX_SEND_LENGTH) {
      LOG_ERROR("Response lenth %zu longer then max send", res_len);
      char *msg = "Internal server error";
      socket_send(fd, msg, strlen(msg) + 1); /* +1: include '\0' terminator */
      continue;
    }
    memcpy(send_buf, result, res_len);
    send_buf[res_len] = '\0'; /* message terminator */
    send_buf[res_len + 1] = '\0';
    if (!socket_send(fd, send_buf, res_len + 1)) {
      LOG_ERROR("Could not send response %s", send_buf);
    }
  }
}

static void *game_loop(void *args) {
  GameLoopArgs *ga = args;
  GameServer *server = ga->server;
  size_t slot = ga->slot;
  char buf[64];
  socket_to_str(&server->registry.clients[slot].socket, buf, sizeof buf);
  LOG_INFO("Session start: slot %zu, client %s", slot, buf);
  server_interact(server->registry.clients[slot].socket.fd, server, slot);
  socket_close(server->registry.clients[slot].socket.fd);
  clients_clear_match(&server->registry, slot);
  disconnect_client(&server->registry, slot);
  free(ga);
  LOG_INFO("Session end: slot %zu freed, client %s", slot, buf);
  return NULL;
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
    int slot = next_free_client_slot(&game_server->registry);
    if (slot < 0) {
      socket_close(client.fd);
      LOG_WARN("Server full (%d clients), rejecting %s", MAX_CLIENTS, buf);
      continue;
    }
    LOG_DEBUG("Claimed client slot %d for %s", slot, buf);
    game_server->registry.clients[slot] =
        (GameClient){.name = {0}, .socket = client};

    GameLoopArgs *args = malloc(sizeof *args);
    args->server = game_server;
    args->slot = (size_t)slot;
    pthread_t tid;
    pthread_create(&tid, NULL, game_loop, args);
    LOG_DEBUG("Started new thread %lu", tid);
    pthread_detach(tid);
  }
}

int main(void) {
  GameServer server;
  if (game_server(&server, 8080)) {
    start(&server);
  }
}
