#ifndef AWALE_SERVER_H

#define AWALE_SERVER_H

#include "src/awale/awale.h"
#include "src/socket.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_CLIENTS 128
#define MAX_SEND_LENGTH 1024

typedef struct GameClient {
  char name[PLAYER_NAME_MAX_LENGTH];
  Socket socket;
} GameClient;

typedef struct GameServer {
  Socket socket;
  GameClient clients[MAX_CLIENTS];
  size_t n_connections;
  pthread_mutex_t clients_lock;
  bool running;
} GameServer;

/* Returns the index of a free client slot, or -1 if the server is full. */
int next_free_client_slot(GameServer *game_server);

/* Marks the client in the given slot as disconnected (fd = -1). */
void disconnect_client(GameServer *game_server, size_t slot);

#endif // !AWALE_SERVER_H
