
#include "src/commands/command.h"
#include <stddef.h>

// game id, move
const ArgKind on_move_args[] = {ARG_INT, ARG_INT};

// game id
const ArgKind on_leave_args[] = {ARG_INT};
const size_t on_move_n_args = 2;
const size_t on_leave_n_args = 1;

const Command commands[] = {
    (Command){
        .cmd = AWALE_MOVE,
        .fn = awale_on_move,
        .arg_types = on_move_args,
        .n_args = on_move_n_args,
    },
    (Command){
        .cmd = LEAVE,
        .fn = awale_on_leave,
        .arg_types = on_leave_args,
        .n_args = on_move_n_args,
    },
};
