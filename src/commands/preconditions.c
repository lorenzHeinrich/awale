#include "src/commands/preconditions.h"
#include "src/clients.h"
#include <stdbool.h>
#include <stddef.h>

static void check_registered(GameClient *client, void *ctx) {
  *(bool *)ctx = client->name[0] != '\0';
}

bool client_is_registered(struct ClientRegistry *registry, size_t client_slot) {
  bool registered = false;
  clients_with_slot(registry, client_slot, check_registered, &registered);
  return registered;
}
