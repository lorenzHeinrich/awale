#include "src/commands/command.h"
#include "arg_parser.h"
#include "src/log.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// pit to move from
const ArgKind on_move_args[] = {ARG_INT};
const size_t on_move_n_args = 1;

// game id
const ArgKind on_leave_args[] = {ARG_INT};
const size_t on_leave_n_args = 1;

// client name
const ArgKind on_register_args[] = {ARG_STR};
const size_t on_register_n_args = 1;

// player name to invite
const ArgKind on_invite_args[] = {ARG_STR};
const size_t on_invite_n_args = 1;

// player name to accept invite from
const ArgKind on_accept_args[] = {ARG_STR};
const size_t on_accept_n_args = 1;

const Command commands[] = {
    (Command){
        .name = "register",
        .fn = on_register,
        .arg_types = on_register_args,
        .n_args = on_register_n_args,
    },
    (Command){
        .name = "move",
        .fn = awale_on_move,
        .arg_types = on_move_args,
        .n_args = on_move_n_args,
    },
    (Command){
        .name = "leave",
        .fn = awale_on_leave,
        .arg_types = on_leave_args,
        .n_args = on_leave_n_args,
    },
    (Command){
        .name = "players",
        .fn = awale_on_players,
        .arg_types = NULL,
        .n_args = 0,
    },
    (Command){
        .name = "help",
        .fn = awale_on_help,
        .arg_types = NULL,
        .n_args = 0,
    },
    (Command){
        .name = "invite",
        .fn = awale_on_invite,
        .arg_types = on_invite_args,
        .n_args = on_invite_n_args,
    },
    (Command){
        .name = "invites",
        .fn = awale_on_invites,
        .arg_types = NULL,
        .n_args = 0,
    },
    (Command){
        .name = "accept",
        .fn = awale_on_accept,
        .arg_types = on_accept_args,
        .n_args = on_accept_n_args,
    },
    (Command){
        .name = "match",
        .fn = awale_on_match,
        .arg_types = NULL,
        .n_args = 0,
    },
};

const size_t n_commands = sizeof commands / sizeof commands[0];

char *exec_cmd(char *args, ClientRegistry *registry, size_t client_slot) {
  char token_bufs[MAX_ARGS][MAX_ARG_LEN];
  char *tokens[MAX_ARGS];
  for (size_t i = 0; i < MAX_ARGS; i++) {
    tokens[i] = token_bufs[i];
  }
  Command cmd = {0};
  if (!parse_cmd(&cmd, tokens, args)) {
    LOG_WARN("slot %zu: could not parse command line \"%s\"", client_slot,
             args);
    return "Parse Error";
  }
  CmdArgs cmd_args = {0};
  if (!parse_args(cmd, tokens, &cmd_args)) {
    LOG_WARN("slot %zu: invalid arguments for command \"%s\"", client_slot,
             cmd.name);
    return "Parse Error";
  }

  LOG_INFO("slot %zu: executing \"%s\"", client_slot, cmd.name);
  char *result = cmd.fn(cmd_args, registry, client_slot);
  LOG_DEBUG("slot %zu: \"%s\" -> \"%s\"", client_slot, cmd.name, result);
  return result;
}
