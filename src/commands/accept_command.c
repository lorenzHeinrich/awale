#include "src/clients.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include "src/match.h"
#include <stddef.h>
#include <string.h>

static void copy_match(GameClient *client, void *ctx) {
  *(Match **)ctx = client->current_match;
}

typedef struct {
  bool started;
  size_t slot_a;
  size_t slot_b;
} MatchInfo;

static void read_match(Match *match, void *ctx) {
  MatchInfo *info = ctx;
  info->started = match->started;
  info->slot_a = match->slot_a;
  info->slot_b = match->slot_b;
}

static void copy_name(GameClient *client, void *ctx) {
  strcpy((char *)ctx, client->name);
}

static void start_match(Match *match, void *ctx) {
  (void)ctx;
  match->started = true;
}

char *awale_on_accept(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot) {
  const char *from = args.argv[0].as.arg_str.str_value;
  if (!client_is_registered(registry, client_slot)) {
    return "Not registered";
  }

  Match *match = NULL;
  clients_with_slot(registry, client_slot, copy_match, &match);
  if (match == NULL) {
    return "no invites";
  }

  MatchInfo info = {0};
  with_match(match, read_match, &info);
  if (info.started) {
    return "match already started";
  }

  size_t inviter_slot = info.slot_a == client_slot ? info.slot_b : info.slot_a;
  char inviter[PLAYER_NAME_MAX_LENGTH] = {0};
  clients_with_slot(registry, inviter_slot, copy_name, inviter);
  if (strcmp(inviter, from) != 0) {
    return "no invite from that player";
  }

  with_match(match, start_match, NULL);
  LOG_INFO("slot %zu accepted invite from %s, match started", client_slot,
           inviter);
  return "Match started";
}
