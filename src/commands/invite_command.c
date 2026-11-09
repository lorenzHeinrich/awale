#include "src/clients.h"
#include "src/commands/command.h"
#include "src/log.h"
#include "src/match.h"
#include <stddef.h>
#include <string.h>

typedef struct {
  const char *target_name;
  size_t target_slot;
  size_t inviter_slot;
  const char *result;
} InviteCtx;

static void find_player(const GameClient *client, size_t slot, void *ctx) {
  InviteCtx *c = ctx;
  if (c->target_slot != (size_t)-1 || client->socket.fd == -1) {
    return;
  }
  if (strcmp(client->name, c->target_name) == 0) {
    c->target_slot = slot;
  }
}

/* Runs under the registry lock with both slots: check + assign atomic. */
static void do_invite(GameClient *inviter, GameClient *invitee, void *ctx) {
  InviteCtx *c = ctx;
  if (inviter->name[0] == '\0') {
    c->result = "Not registered";
    return;
  }
  if (invitee->name[0] == '\0') {
    c->result = "Player not registered";
    return;
  }
  if (inviter->current_match != NULL) {
    c->result = "Already in a match";
    return;
  }
  if (invitee->current_match != NULL) {
    c->result = "Player already in a match";
    return;
  }
  Match *m =
      match_new(inviter->name, invitee->name, c->inviter_slot, c->target_slot);
  inviter->current_match = m;
  invitee->current_match = m;
  c->result = "Invite sent";
  LOG_INFO("slot %zu invited slot %zu (%s -> %s), match pending",
           c->inviter_slot, c->target_slot, inviter->name, invitee->name);
}

char *awale_on_invite(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot) {
  const char *target = args.argv[0].as.arg_str.str_value;
  InviteCtx ctx = {.target_name = target,
                   .target_slot = (size_t)-1,
                   .inviter_slot = client_slot,
                   .result = NULL};
  clients_for_each(registry, find_player, &ctx);
  if (ctx.target_slot == (size_t)-1) {
    return "No such player";
  }
  if (ctx.target_slot == client_slot) {
    return "Cannot invite yourself";
  }
  clients_with_two_slots(registry, client_slot, ctx.target_slot, do_invite,
                         &ctx);
  return (char *)ctx.result;
}
