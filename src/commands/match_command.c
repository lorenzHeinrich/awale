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
  bool started;
  size_t slot_a;
  size_t slot_b;
  int rendered; /* game_to_str result, if board was rendered */
  char *resp;
  size_t resp_len;
} MatchCtx;

static void render_match(Match *match, void *ctx) {
  MatchCtx *c = ctx;
  c->started = match->started;
  c->slot_a = match->slot_a;
  c->slot_b = match->slot_b;
  if (match->started) {
    c->rendered = game_to_str(&match->game, c->resp, c->resp_len);
  }
}

static void copy_name(GameClient *client, void *ctx) {
  strcpy((char *)ctx, client->name);
}

char *awale_on_match(const CmdArgs args, ClientRegistry *registry,
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

  MatchCtx ctx = {.resp = resp, .resp_len = sizeof resp, .rendered = 0};
  with_match(match, render_match, &ctx);
  if (!ctx.started) {
    size_t other = ctx.slot_a == client_slot ? ctx.slot_b : ctx.slot_a;
    char inviter[PLAYER_NAME_MAX_LENGTH] = {0};
    clients_with_slot(registry, other, copy_name, inviter);
    snprintf(resp, sizeof resp, "invite from %s pending\n", inviter);
    return resp;
  }
  return resp;
}
