#ifndef COMMAND_H
#define COMMAND_H

#include "src/clients.h"
#include <stddef.h>

#define CMD_LEN 32
#define MAX_ARG_LEN 64
#define MAX_ARGS 4
#define MAX_INPUT_LEN CMD_LEN + (MAX_ARGS * MAX_ARG_LEN)

typedef enum { ARG_VOID, ARG_INT, ARG_STR } ArgKind;

typedef struct ArgVoid {
  void *no_value;
} ArgVoid;

typedef struct ArgInt {
  int int_value;
} ArgInt;

typedef struct ArgStr {
  char *str_value;
} ArgStr;

typedef struct CmdArg {
  ArgKind kind;
  union {
    struct ArgVoid arg_void;
    struct ArgInt arg_int;
    struct ArgStr arg_str;
  } as;
} CmdArg;

typedef struct CmdArgs {
  size_t argc;
  CmdArg argv[MAX_ARGS];
} CmdArgs;

typedef char *(*CmdFn)(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot);

char *exec_cmd(char *args, ClientRegistry *registry, size_t client_slot);

typedef struct {
  char *name;
  CmdFn fn;
  const ArgKind *arg_types;
  size_t n_args;
} Command;

extern char *awale_on_move(const CmdArgs args, ClientRegistry *registry,
                           size_t client_slot);
extern const ArgKind on_move_args[];
extern const size_t on_move_n_args;

char *awale_on_leave(const CmdArgs args, ClientRegistry *registry,
                     size_t client_slot);

char *awale_on_players(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot);

char *awale_on_help(const CmdArgs args, ClientRegistry *registry,
                    size_t client_slot);

char *awale_on_invite(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot);
extern const ArgKind on_invite_args[];
extern const size_t on_invite_n_args;

char *awale_on_invites(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot);

char *awale_on_accept(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot);
extern const ArgKind on_accept_args[];
extern const size_t on_accept_n_args;

char *awale_on_match(const CmdArgs args, ClientRegistry *registry,
                     size_t client_slot);

char *awale_on_history(const CmdArgs args, ClientRegistry *registry,
                       size_t client_slot);

char *awale_on_replay(const CmdArgs args, ClientRegistry *registry,
                      size_t client_slot);
extern const ArgKind on_replay_args[];
extern const size_t on_replay_n_args;

char *awale_on_surrender(const CmdArgs args, ClientRegistry *registry,
                         size_t client_slot);

char *on_register(const CmdArgs args, ClientRegistry *registry,
                  size_t client_slot);
extern const ArgKind on_register_args[];
extern const size_t on_register_n_args;

extern const Command commands[];
extern const size_t n_commands;

#endif // !COMMAND_H
