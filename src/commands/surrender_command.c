#include "src/awale/awale.h"
#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include "src/match.h"
#include <stdio.h>
#include <string.h>

static void copy_match(GameClient *client, void *ctx) {
  *(Match **)ctx = client->current_match;
}

typedef struct {
  size_t client_slot;
  const char *error;
  bool started;
  Side opponent_side;
  char opponent[PLAYER_NAME_MAX_LENGTH];
} SurrenderCtx;

static void do_surrender(Match *match, void *ctx) {
  SurrenderCtx *c = ctx;
  c->started = match->started;
  if (!match->started) {
    c->error = "match not started yet";
    return;
  }
  Side side = match->slot_a == c->client_slot ? A : B;
  c->opponent_side = side == A ? B : A;
  match->game.winner = c->opponent_side;
  match->game.has_winner = true;
  match->game.game_over = true;
}

static void clear_match_ptr(GameClient *a, GameClient *b, void *ctx) {
  (void)ctx;
  a->current_match = NULL;
  b->current_match = NULL;
}

static void copy_name(GameClient *client, void *ctx) {
  strcpy((char *)ctx, client->name);
}

char *awale_on_surrender(const CmdArgs args, ClientRegistry *registry,
                         size_t client_slot) {
  (void)args;
  static _Thread_local char resp[MAX_SEND_LENGTH];
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }

  Match *match = NULL;
  clients_with_slot(registry, client_slot, copy_match, &match);
  if (match == NULL) {
    return "no match";
  }

  SurrenderCtx ctx = {.client_slot = client_slot, .error = NULL};
  with_match(match, do_surrender, &ctx);
  if (ctx.error != NULL) {
    return (char *)ctx.error;
  }

  size_t opponent_slot =
      match->slot_a == client_slot ? match->slot_b : match->slot_a;
  clients_with_slot(registry, opponent_slot, copy_name, ctx.opponent);

  match_record(registry, match);
  clients_with_two_slots(registry, match->slot_a, match->slot_b,
                         clear_match_ptr, NULL);
  match_destroy(match);

  snprintf(resp, sizeof resp, "Surrendered, %s wins\n", ctx.opponent);
  LOG_INFO("slot %zu surrendered, %s wins", client_slot, ctx.opponent);
  return resp;
}
