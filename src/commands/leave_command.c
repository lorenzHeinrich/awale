#include "src/commands/command.h"
#include "src/log.h"

char *awale_on_leave(const CmdArgs args) {
  int game_id = args.argv[0].as.arg_int.int_value;

  LOG_DEBUG("leave game %d", game_id);
}
