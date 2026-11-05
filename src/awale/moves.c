#include "src/awale/moves.h"
#include "src/awale/board.h"
#include <stddef.h>

static int _sow(Board *board, int start_pit);
static MoveCheck _check_catch(Board *board, int pit, Side side);
static int _catch(Board *board, int pit, Side side);

MoveResult sow_and_catch(Board *board, Side side, int start_pit) {
  MoveCheck sow_check = check_sow(board, side, start_pit);
  if (!sow_check) {
    return (MoveResult){sow_check, 0};
  }
  int last_pit = _sow(board, start_pit);
  if (is_side(side, last_pit)) {
    return (MoveResult){MOVE_OK, 0};
  }
  Side other = other_side(side);
  MoveCheck catch_check = _check_catch(board, last_pit, other);
  if (!catch_check) {
    return (MoveResult){catch_check, 0};
  }
  int obtained = _catch(board, last_pit, other);
  return (MoveResult){MOVE_OK, obtained};
}

MoveCheck check_sow(Board *board, Side side, int start_pit) {
  if (!check_pit(start_pit)) {
    return PIT_INDEX_ERROR;
  }
  if (!is_side(side, start_pit)) {
    return NOT_SIDES_PIT;
  }
  if (board->pits[start_pit] == 0) {
    return PIT_EMPTY;
  }
  if (!sides_is_empty(board, other_side(side))) {
    return MOVE_OK;
  }
  int distance = side + PITS_PER_SIDE - start_pit;
  return board->pits[start_pit] < distance ? NOT_A_FEED : MOVE_OK;
}

static int _sow(Board *board, int start_pit) {
  int stones = take_stones(board, start_pit);
  for (int i = 1; i <= stones; i++) {
    put_stone(board, (start_pit + i) % N_PITS);
  }
  return (start_pit + stones) % N_PITS;
}

static MoveCheck _check_catch(Board *board, int pit, Side side) {
  int total_stones = 0;
  for (size_t i = side; i < side + PITS_PER_SIDE; i++) {
    total_stones += board->pits[i];
  }
  int would_capture = 0;
  for (int i = pit; is_side(side, i); i--) {
    if (!TWO_OR_THREE(board->pits[i])) {
      break;
    }
    would_capture += board->pits[i];
  }
  return would_capture > 0 && total_stones == would_capture ? GRAND_SLAM
                                                            : MOVE_OK;
}

static int _catch(Board *board, int pit, Side side) {
  if (!is_side(side, pit) || !TWO_OR_THREE(board->pits[pit])) {
    return 0;
  }
  return take_stones(board, pit) + _catch(board, PREV_PIT(pit), side);
}
