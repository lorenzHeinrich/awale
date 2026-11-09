#include "src/awale/awale.h"
#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include <stdio.h>

typedef struct {
  size_t id;
  const char *error;
  char *resp;
  size_t resp_len;
} ReplayCtx;

static void render_entry(GameClient *client, void *ctx) {
  ReplayCtx *c = ctx;
  if (c->id >= client->n_history) {
    c->error = "no such history entry";
    return;
  }
  MatchRecord *rec = &client->history[c->id];
  int n = game_to_str(&rec->game, c->resp, c->resp_len);
  size_t off = n < 0 ? 0 : (size_t)n;
  const char *winner = "draw";
  if (rec->game.has_winner) {
    winner = player_for_side(&rec->game, rec->game.winner)->name;
  }
  snprintf(c->resp + off, c->resp_len - off, "vs %s, winner: %s\n",
           rec->opponent, winner);
}

char *awale_on_replay(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot) {
  static _Thread_local char resp[MAX_SEND_LENGTH];
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }
  int id = args.argv[0].as.arg_int.int_value;
  if (id < 0) {
    return "no such history entry";
  }
  ReplayCtx ctx = {
      .id = (size_t)id, .error = NULL, .resp = resp, .resp_len = sizeof resp};
  clients_with_slot(registry, client_slot, render_entry, &ctx);
  if (ctx.error != NULL) {
    return (char *)ctx.error;
  }
  return resp;
}
