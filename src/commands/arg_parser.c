#include "src/commands/arg_parser.h"
#include "src/commands/command.h"
#include "src/log.h"
#include "src/utils.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool parse_cmd(Command *cmd, char **tokens, char *args) {
  LOG_DEBUG("Receiving \"%s\" to parse", args);
  char copy[MAX_INPUT_LEN + 2];
  int n = snprintf(copy, sizeof(copy), "%s", args);

  if (n < 0 || (size_t)n >= sizeof copy) {
    LOG_DEBUG("Command line too long: %s", args);
    return false;
  }

  char *toks[MAX_ARGS + 1];
  char *tok = strtok(copy, " ");
  size_t token_count = 0;
  while (token_count <= MAX_ARGS && tok != NULL) {
    LOG_DEBUG("Token %s", tok);
    toks[token_count] = tok;
    token_count++;
    tok = strtok(NULL, " "); /* NULL = continue the previous scan */
  }

  if (token_count > MAX_ARGS) {
    LOG_DEBUG("Too many arguments in %s", args);
    return false;
  }

  if (token_count == 0) {
    LOG_DEBUG("No tokens found in %s", args);
    return false;
  }

  for (size_t i = 0; i < n_commands; i++) {
    // strcmp returns 0 on equal
    if (!strcmp(toks[0], commands[i].name)) {
      *cmd = commands[i];
      LOG_DEBUG("Parsed to command %s", cmd->name);
      if (cmd->n_args != token_count - 1) {
        LOG_DEBUG("Expected %zu args but got %zu", cmd->n_args,
                  token_count - 1);
        return false;
      }
      for (size_t j = 0; j < token_count - 1; j++) {
        size_t len = strlen(toks[j + 1]);
        if (len >= MAX_ARG_LEN) {
          LOG_DEBUG("Argument too long: %s", toks[j + 1]);
          return false;
        }

        memcpy(tokens[j], toks[j + 1], len + 1);
      }
      return true;
    }
  }
  LOG_DEBUG("Could not find command in %s", args);
  return false;
}

bool parse_args(const Command cmd, char **tokens, CmdArgs *cmd_args) {
  size_t argc;
  for (argc = 0; argc < cmd.n_args; argc++) {
    switch (cmd.arg_types[argc]) {
    case ARG_INT: {
      int int_arg;
      if (!parse_int(tokens[argc], &int_arg, 0, 11)) {
        LOG_DEBUG("cannot parse %s as int arg", tokens[argc]);
        return false;
      }
      cmd_args->argv[argc] = (CmdArg){
          .kind = ARG_INT,
          .as = {.arg_int = {.int_value = int_arg}},
      };
      break;
    }
    case ARG_STR: {
      if (!parse_str(tokens[argc], 1, MAX_ARG_LEN)) {
        LOG_DEBUG("cannot parse %s as string arg", tokens[argc]);
        return false;
      }
      cmd_args->argv[argc] = (CmdArg){
          .kind = ARG_STR,
          .as = {.arg_str = {.str_value = tokens[argc]}},
      };
      break;
    }
    case ARG_VOID:
      cmd_args->argv[argc] = (CmdArg){
          .kind = ARG_VOID,
          .as = {{((void *)0)}},
      };
      break;
    }
  }
  cmd_args->argc = argc;
  return true;
}

bool parse_int(const char *s, int *i, int min, int max) {
  char *end;
  errno = 0;
  long v = strtol(s, &end, 10);
  if (end == s || *end != '\0')
    return false;
  if (errno == ERANGE || v < min || v > max)
    return false;
  *i = (int)v;
  return true;
}

bool parse_str(const char *s, size_t min, size_t max) {
  size_t len = strnlen(s, max + 1);
  if (len > max || len < min) {
    return false;
  }
  return true;
}
