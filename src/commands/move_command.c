#include "src/awale/awale.h"
#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include "src/match.h"
#include <stddef.h>

static void copy_match(GameClient *client, void *ctx) {
  *(Match **)ctx = client->current_match;
}

typedef struct {
  int pit;
  size_t client_slot;
  const char *error; /* set on failure; NULL = board rendered into resp */
  bool game_over;
  char *resp;
  size_t resp_len;
} MoveCtx;

static void read_game_over(Match *match, void *ctx) {
  *(bool *)ctx = match->game.game_over;
}

static void clear_match_ptr(GameClient *a, GameClient *b, void *ctx) {
  (void)ctx;
  a->current_match = NULL;
  b->current_match = NULL;
}

static void do_move(Match *match, void *ctx) {
  MoveCtx *c = ctx;
  if (!match->started) {
    c->error = "match not started yet";
    return;
  }
  Side side = match->slot_a == c->client_slot ? A : B;
  if (match->game.turn != side) {
    c->error = "not your turn";
    return;
  }
  MoveResult r = move(&match->game, c->pit);
  if (r.move_check != MOVE_OK) {
    c->error = "illegal move";
    return;
  }
  LOG_INFO("slot %zu (side %c) moved pit %d, captured %d", c->client_slot,
           side == A ? 'A' : 'B', c->pit, r.obtained);
  c->game_over = match->game.game_over;
  game_to_str(&match->game, c->resp, c->resp_len);
}

char *awale_on_move(const CmdArgs args, ClientRegistry *registry,
                    size_t client_slot) {
  static _Thread_local char resp[MAX_SEND_LENGTH];
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }
  int pit = args.argv[0].as.arg_int.int_value;

  Match *match = NULL;
  clients_with_slot(registry, client_slot, copy_match, &match);
  if (match == NULL) {
    return "no match";
  }

  MoveCtx ctx = {.pit = pit,
                 .client_slot = client_slot,
                 .error = NULL,
                 .game_over = false,
                 .resp = resp,
                 .resp_len = sizeof resp};
  with_match(match, do_move, &ctx);
  if (ctx.error != NULL) {
    return (char *)ctx.error;
  }
  if (ctx.game_over) {
    /* Game finished: record final state in both histories, release match. */
    match_record(registry, match);
    clients_with_two_slots(registry, match->slot_a, match->slot_b,
                           clear_match_ptr, NULL);
    match_destroy(match);
    LOG_INFO("slot %zu: game over, recorded to history", client_slot);
  }
  return resp;
}
