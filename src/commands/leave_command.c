#include "src/commands/command.h"
#include "src/log.h"
#include <stddef.h>

char *awale_on_leave(const CmdArgs args, ClientRegistry *registry,
                     size_t client_slot) {
  int game_id = args.argv[0].as.arg_int.int_value;

  LOG_DEBUG("leave game %d", game_id);
  (void)registry;
  return "Leaving...";
}
