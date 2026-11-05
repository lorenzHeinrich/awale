#ifndef MOVES_H

#define MOVES_H

#include "board.h"
#include <stdbool.h>

typedef enum {
  MOVE_OK = true,
  PIT_INDEX_ERROR = false,
  PIT_EMPTY = false,
  NOT_SIDES_PIT = false,
  NOT_A_FEED = false,
  GRAND_SLAM = false,
  NOT_YOUR_TURN = false,
  GAME_OVER = false,
} MoveCheck;

typedef struct {
  MoveCheck move_check;
  int obtained;
} MoveResult;

MoveResult sow_and_catch(Board *board, Side side, int start_pit);

MoveCheck check_sow(Board *board, Side side, int start_pit);

MoveCheck _check_catch(Board *board, int pit, Side side);

int _sow(Board *board, int start_pit);

int _catch(Board *board, int pit, Side side);

#endif // !MOVES_H
