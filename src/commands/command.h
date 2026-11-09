#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

#define CMD_LEN 32
#define MAX_ARG_LEN 64
#define MAX_ARGS 4
#define MAX_INPUT_LEN CMD_LEN + (MAX_ARGS * MAX_ARG_LEN)

typedef enum { ARG_VOID, ARG_INT, ARG_STR } ArgKind;

typedef struct ArgVoid {
  void* no_value;
} ArgVoid;

typedef struct ArgInt {
  int int_value;
} ArgInt;

typedef struct ArgStr {
  char* str_value;
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

typedef void (*CmdFn)(const CmdArgs args);

void exec_cmd(char* args);

typedef struct {
  char* name;
  CmdFn fn;
  const ArgKind* arg_types;
  size_t n_args;
} Command;

extern void awale_on_move(const CmdArgs args);
extern const ArgKind on_move_args[];
extern const size_t on_move_n_args;

void awale_on_leave(const CmdArgs args);
extern const ArgKind on_leave_args[];
extern const size_t on_leave_n_args;

extern void on_register(const CmdArgs args);
extern const ArgKind on_register_args[];
extern const size_t on_register_n_args;

extern const Command commands[];
extern const size_t n_commands;

#endif // !COMMAND_H
