#ifndef AWALE_SERVER_H

#define AWALE_SERVER_H

#include "src/socket.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_CLIENTS 128

typedef struct GameClient {
  Socket socket;
} GameClient;

typedef struct GameServer {
  Socket socket;
  GameClient clients[MAX_CLIENTS];
  size_t n_connections;
  bool running;
} GameServer;

#endif // !AWALE_SERVER_H
