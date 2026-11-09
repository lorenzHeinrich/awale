#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include <stdio.h>

typedef struct {
  char *resp;
  size_t off;
  bool full;
} ListCtx;

static void append_player(const GameClient *client, size_t slot, void *ctx) {
  (void)slot;
  ListCtx *c = ctx;
  if (c->full || client->socket.fd == -1) {
    return; /* free slot */
  }
  const char *name = client->name[0] != '\0' ? client->name : "(unnamed)";
  int n = snprintf(c->resp + c->off, MAX_SEND_LENGTH - c->off, "%s\n", name);
  if (n < 0 || (size_t)n >= MAX_SEND_LENGTH - c->off) {
    c->off = MAX_SEND_LENGTH - 1; /* truncated */
    c->full = true;
    return;
  }
  c->off += (size_t)n;
}

char *awale_on_players(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot) {
  (void)args;
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }
  /* Thread-local so concurrent game threads don't clobber each other. */
  static _Thread_local char resp[MAX_SEND_LENGTH];
  ListCtx ctx = {.resp = resp, .off = 0, .full = false};
  clients_for_each(registry, append_player, &ctx);
  if (ctx.off == 0) {
    return "No players connected";
  }
  resp[ctx.off] = '\0';
  return resp;
}
