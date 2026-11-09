#ifndef AWALE_SERVER_H

#define AWALE_SERVER_H

#include "src/clients.h"
#include "src/socket.h"
#include <stdbool.h>

typedef struct GameServer {
  Socket socket;
  ClientRegistry registry;
  bool running;
} GameServer;

#endif // !AWALE_SERVER_H
