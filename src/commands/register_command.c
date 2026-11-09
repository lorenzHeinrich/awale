#include "src/awale_server.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include <stddef.h>
#include <string.h>

char *on_register(const CmdArgs args, GameServer *server, size_t client_slot) {
  char *name = args.argv[0].as.arg_str.str_value;
  LOG_DEBUG("trying to register client %s", name);
  if (client_is_registered(server, client_slot)) {
    return "Already registered";
  }
  if (strlen(name) >= PLAYER_NAME_MAX_LENGTH) {
    return "Name too long";
  }
  strcpy(server->clients[client_slot].name, name);
  return "Registered";
}
