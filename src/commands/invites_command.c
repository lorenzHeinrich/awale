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

char *awale_on_invites(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot) {
  (void)args;
  static _Thread_local char resp[MAX_SEND_LENGTH];
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
    return "no invites";
  }

  size_t other = info.slot_a == client_slot ? info.slot_b : info.slot_a;
  char inviter[PLAYER_NAME_MAX_LENGTH] = {0};
  clients_with_slot(registry, other, copy_name, inviter);
  snprintf(resp, sizeof resp, "invite from %s\n", inviter);
  LOG_DEBUG("slot %zu: listing invite from %s", client_slot, inviter);
  return resp;
}
