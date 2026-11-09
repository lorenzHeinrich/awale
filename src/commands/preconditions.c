#include "src/commands/preconditions.h"
#include "src/awale_server.h"
#include <stdbool.h>
#include <stddef.h>

bool client_is_registered(struct GameServer *server, size_t client_slot) {
  return server->clients[client_slot].name[0] != '\0';
}
