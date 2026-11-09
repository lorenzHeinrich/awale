#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include <stdio.h>

typedef struct {
  char *resp;
  size_t off;
  bool full;
} HistoryCtx;

static void append_entry(GameClient *client, void *ctx) {
  HistoryCtx *c = ctx;
  for (size_t i = 0; i < client->n_history && !c->full; i++) {
    MatchRecord *rec = &client->history[i];
    const char *winner = "draw";
    if (rec->game.has_winner) {
      Player *w = player_for_side(&rec->game, rec->game.winner);
      winner = w->name;
    }
    int n = snprintf(c->resp + c->off, MAX_SEND_LENGTH - c->off,
                     "#%zu vs %s, winner: %s\n", i, rec->opponent, winner);
    if (n < 0 || (size_t)n >= MAX_SEND_LENGTH - c->off) {
      c->full = true;
      return;
    }
    c->off += (size_t)n;
  }
}

char *awale_on_history(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot) {
  (void)args;
  static _Thread_local char resp[MAX_SEND_LENGTH];
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }
  HistoryCtx ctx = {.resp = resp, .off = 0, .full = false};
  clients_with_slot(registry, client_slot, append_entry, &ctx);
  if (ctx.off == 0) {
    return "no history";
  }
  resp[ctx.off] = '\0';
  return resp;
}
