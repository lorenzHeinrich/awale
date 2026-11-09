#include "src/commands/command.h"
#include "arg_parser.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// game id, move
const ArgKind on_move_args[] = {ARG_INT, ARG_INT};
const size_t on_move_n_args = 2;

// game id
const ArgKind on_leave_args[] = {ARG_INT};
const size_t on_leave_n_args = 1;

// client name
const ArgKind on_register_args[] = {ARG_STR};
const size_t on_register_n_args = 1;

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
};

const size_t n_commands = sizeof commands / sizeof commands[0];

char *exec_cmd(char *args) {
  char token_bufs[MAX_ARGS][MAX_ARG_LEN];
  char *tokens[MAX_ARGS];
  for (size_t i = 0; i < MAX_ARGS; i++) {
    tokens[i] = token_bufs[i];
  }
  Command cmd = {0};
  if (!parse_cmd(&cmd, tokens, args)) {
    return "Parse Error";
  }
  CmdArgs cmd_args = {0};
  if (!parse_args(cmd, tokens, &cmd_args)) {
    return "Parse Error";
  }

  return cmd.fn(cmd_args);
}
