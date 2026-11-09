#include "src/awale_server.h"
#include "src/commands/command.h"
#include "src/commands/preconditions.h"
#include "src/log.h"
#include <pthread.h>
#include <stdio.h>

char* awale_on_players(const CmdArgs args, GameServer* server,
                       size_t client_slot) {
  (void)args;
  if (!client_is_registered(server, client_slot)) {
    return "Not registered";
  }
  /* Thread-local so concurrent game threads don't clobber each other. */
  static _Thread_local char resp[MAX_SEND_LENGTH];
  size_t off = 0;

  pthread_mutex_lock(&server->clients_lock);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (server->clients[i].socket.fd == -1) {
      continue; /* free slot */
    }
    const char* name = server->clients[i].name[0] != '\0'
                           ? server->clients[i].name
                           : "(unnamed)";
    /* Multiline is fine: messages are '\0'-terminated, not newline-delimited.
     */
    int n = snprintf(resp + off, MAX_SEND_LENGTH - off, "%s\n", name);
    if (n < 0 || (size_t)n >= MAX_SEND_LENGTH - off) {
      off = MAX_SEND_LENGTH - 1; /* truncated */
      break;
    }
    off += (size_t)n;
  }
  pthread_mutex_unlock(&server->clients_lock);

  if (off == 0) {
    return "No players connected";
  }
  resp[off] = '\0';
  return resp;
}
