#include "awale_server.h"
#include "commands/arg_parser.h"
#include "commands/command.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* Stubs for the command handlers referenced by the commands[] table. */
char* on_register(const CmdArgs args, GameServer* server, size_t client_slot) {
  return "";
}
char* awale_on_move(const CmdArgs args, GameServer* server,
                    size_t client_slot) {
  return "";
}
char* awale_on_leave(const CmdArgs args, GameServer* server,
                     size_t client_slot) {
  return "";
}
char* awale_on_players(const CmdArgs args, GameServer* server,
                       size_t client_slot) {
  return "";
}
char* awale_on_help(const CmdArgs args, GameServer* server,
                    size_t client_slot) {
  return "";
}

static void test_parse_cmd_move(void) {
  Command cmd;
  char token_bufs[MAX_ARGS][MAX_ARG_LEN];
  char* tokens[MAX_ARGS];
  for (size_t i = 0; i < MAX_ARGS; i++) {
    tokens[i] = token_bufs[i];
  }
  char line[] = "move 1 2";

  assert(parse_cmd(&cmd, tokens, line));
  assert(strcmp(cmd.name, "move") == 0);
  assert(cmd.n_args == 2);
  assert(strcmp(tokens[0], "1") == 0);
  assert(strcmp(tokens[1], "2") == 0);
}

static void test_parse_args_move(void) {
  Command cmd = {
      .name = "move",
      .fn = awale_on_move,
      .arg_types = on_move_args,
      .n_args = on_move_n_args,
  };
  char t0[] = "1";
  char t1[] = "2";
  char* tokens[] = {t0, t1};
  CmdArgs args = {0};

  assert(parse_args(cmd, tokens, &args));
  assert(args.argc == 2);
  assert(args.argv[0].kind == ARG_INT);
  assert(args.argv[0].as.arg_int.int_value == 1);
  assert(args.argv[1].kind == ARG_INT);
  assert(args.argv[1].as.arg_int.int_value == 2);
}

static void test_parse_cmd_unknown(void) {
  Command cmd;
  char* tokens[MAX_ARGS] = {0};
  char line[] = "bogus 1 2";

  assert(!parse_cmd(&cmd, tokens, line));
}

static void test_parse_cmd_wrong_argc(void) {
  Command cmd;
  char* tokens[MAX_ARGS] = {0};
  char line[] = "move 1 2 3";

  assert(!parse_cmd(&cmd, tokens, line));
}

static void test_parse_cmd_too_many_tokens(void) {
  Command cmd;
  char* tokens[MAX_ARGS] = {0};
  char line[] = "move 1 2 3 4 5";

  assert(!parse_cmd(&cmd, tokens, line));
}

static void test_parse_cmd_empty(void) {
  Command cmd;
  char* tokens[MAX_ARGS] = {0};
  char line[] = "";

  assert(!parse_cmd(&cmd, tokens, line));
}

static void test_parse_args_non_int(void) {
  Command cmd = {
      .name = "move",
      .fn = awale_on_move,
      .arg_types = on_move_args,
      .n_args = on_move_n_args,
  };
  char t0[] = "abc";
  char t1[] = "2";
  char* tokens[] = {t0, t1};
  CmdArgs args = {0};

  assert(!parse_args(cmd, tokens, &args));
}

static void test_parse_args_int_out_of_range(void) {
  Command cmd = {
      .name = "move",
      .fn = awale_on_move,
      .arg_types = on_move_args,
      .n_args = on_move_n_args,
  };
  char t0[] = "12"; /* max is 11 */
  char t1[] = "2";
  char* tokens[] = {t0, t1};
  CmdArgs args = {0};

  assert(!parse_args(cmd, tokens, &args));
}

static void test_parse_args_empty_str(void) {
  Command cmd = {
      .name = "register",
      .fn = on_register,
      .arg_types = on_register_args,
      .n_args = on_register_n_args,
  };
  char t0[] = "";
  char* tokens[] = {t0};
  CmdArgs args = {0};

  assert(!parse_args(cmd, tokens, &args));
}

/* An optional case name lets Meson report failures independently. */
int main(int argc, char** argv) {
  if (argc == 1 || strcmp(argv[1], "parse_cmd") == 0) {
    test_parse_cmd_move();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_args") == 0) {
    test_parse_args_move();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_cmd_unknown") == 0) {
    test_parse_cmd_unknown();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_cmd_wrong_argc") == 0) {
    test_parse_cmd_wrong_argc();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_cmd_too_many_tokens") == 0) {
    test_parse_cmd_too_many_tokens();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_cmd_empty") == 0) {
    test_parse_cmd_empty();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_args_non_int") == 0) {
    test_parse_args_non_int();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_args_int_out_of_range") == 0) {
    test_parse_args_int_out_of_range();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "parse_args_empty_str") == 0) {
    test_parse_args_empty_str();
    if (argc > 1) {
      return 0;
    }
  }
  return 0;
}
