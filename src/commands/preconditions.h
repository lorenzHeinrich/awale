#ifndef PRECONDITIONS_H
#define PRECONDITIONS_H

#include "src/commands/command.h"
#include <stdbool.h>
#include <stddef.h>

bool client_is_registered(struct GameServer *server, size_t client_slot);

#endif // !PRECONDITIONS_H
