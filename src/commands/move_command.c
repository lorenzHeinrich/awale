#include "src/commands/command.h"
#include "src/log.h"

void awale_on_move(const CmdArgs args) {
  int game_id = args.argv[0].as.arg_int.int_value;
  int move = args.argv[0].as.arg_int.int_value;

  LOG_DEBUG("move %d in game %d", move, game_id);
}
