#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "src/commands/command.h"
#include <stdbool.h>

bool parse_cmd(Command *cmd, char **tokens, char *args);

bool parse_args(const Command cmd, char **tokens, CmdArgs *cmd_args);

#endif // !ARG_PARSER_H
