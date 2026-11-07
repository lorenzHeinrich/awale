#ifndef AWALE_SERVER_H

#define AWALE_SERVER_H

#include "src/socket.h"
#include <stdbool.h>

typedef struct GameServer {
  Socket socket;
  bool running;
} GameServer;

typedef struct GameClient {
  Socket socket;
} GameClient;

#endif // !AWALE_SERVER_H
