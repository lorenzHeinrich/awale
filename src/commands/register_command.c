#include "src/commands/command.h"
#include "src/log.h"

char *on_register(const CmdArgs args) {
  char *name = args.argv[0].as.arg_str.str_value;

  LOG_DEBUG("registering client %s", name);
  return "Registering...";
}
