#include "src/commands/command.h"
#include "src/log.h"
#include <stddef.h>

char *awale_on_move(const CmdArgs args, GameServer *server,
                    size_t client_slot) {
  int game_id = args.argv[0].as.arg_int.int_value;
  int move = args.argv[0].as.arg_int.int_value;

  LOG_DEBUG("move %d in game %d", move, game_id);
  return "makeing move ...";
}
