#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "src/commands/command.h"
#include <stdbool.h>

bool parse_cmd(Command *cmd, char **tokens, char *args);

bool parse_args(const Command cmd, char **tokens, CmdArgs *cmd_args);

bool parse_int(const char *s, int *i, int min, int max);

bool parse_str(const char *s, size_t min, size_t max);

#endif // !ARG_PARSER_H
