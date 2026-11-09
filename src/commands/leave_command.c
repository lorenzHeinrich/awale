#include "src/clients.h"
#include "src/commands/command.h"
#include "src/log.h"
#include <stddef.h>

static void set_quit(GameClient *client, void *ctx) {
  (void)ctx;
  client->quit = true;
}

char *awale_on_leave(const CmdArgs args, ClientRegistry *registry,
                     size_t client_slot) {
  (void)args;
  /* game_loop does the actual cleanup (close fd, end match, free slot)
     when server_interact returns. */
  clients_with_slot(registry, client_slot, set_quit, NULL);
  LOG_INFO("slot %zu: leave requested, ending session", client_slot);
  return "Bye";
}
