#include "src/clients.h"
#include "src/commands/command.h"
#include "src/log.h"
#include <stddef.h>
#include <string.h>

typedef struct {
  const char *name;
  const char *result;
} RegisterCtx;

/* Runs under clients_lock: the check and the write are atomic. */
static void do_register(GameClient *client, void *ctx) {
  RegisterCtx *c = ctx;
  if (client->name[0] != '\0') {
    c->result = "Already registered";
    return;
  }
  if (strlen(c->name) >= PLAYER_NAME_MAX_LENGTH) {
    c->result = "Name too long";
    return;
  }
  strcpy(client->name, c->name);
  c->result = "Registered";
}

char *on_register(const CmdArgs args, ClientRegistry *registry,
                  size_t client_slot) {
  char *name = args.argv[0].as.arg_str.str_value;
  LOG_DEBUG("trying to register client %s", name);
  RegisterCtx ctx = {.name = name, .result = NULL};
  clients_with_slot(registry, client_slot, do_register, &ctx);
  return (char *)ctx.result;
}
