#include "src/commands/command.h"
#include "src/log.h"
#include <stdio.h>

char* awale_on_help(const CmdArgs args, GameServer* server,
                    size_t client_slot) {
  (void)args;
  (void)server;
  (void)client_slot;
  static _Thread_local char resp[MAX_SEND_LENGTH];
  size_t off = 0;

  for (size_t i = 0; i < n_commands; i++) {
    int n =
        snprintf(resp + off, MAX_SEND_LENGTH - off, "%s\n", commands[i].name);
    if (n < 0 || (size_t)n >= MAX_SEND_LENGTH - off) {
      off = MAX_SEND_LENGTH - 1; /* truncated */
      break;
    }
    off += (size_t)n;
  }
  resp[off] = '\0';
  return resp;
}
