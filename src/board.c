#include "board.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool is_side(Side side, int pit) {
  int first_pit = (int)side;
  return pit >= first_pit && pit < first_pit + PITS_PER_SIDE;
}

Side other_side(Side side) { return side == A ? B : A; }

bool check_take_stones(Board *board, Side side, int pit) {
  if (!is_side(side, pit)) {
    return NOT_SIDES_PIT;
  }
  if (pit >= N_PITS) {
    return PIT_INDEX_ERROR;
  }
  if (board->pits[pit] == 0) {
    return PIT_EMPTY;
  }
  return MOVE_OK;
}

static TakeStonesResult take_stones(Board *board, Side side, int pit) {
  MoveCheck move_check = check_take_stones(board, side, pit);
  int stones = 0;
  if (move_check) {
    stones = board->pits[pit];
    board->pits[pit] = 0;
  }
  return (TakeStonesResult){.move_check = move_check, .stones = stones};
}

static void put_stone(Board *board, int pit) { board->pits[pit]++; }

MoveCheck check_sow(Board *board, Side side, int start_pit) {
  bool other_side_empty = true;
  size_t other = other_side(side);
  for (size_t i = other; i < other + PITS_PER_SIDE; i++) {
    if (board->pits[i] > 0) {
      other_side_empty = false;
      break;
    }
  }
  if (!other_side_empty) {
    return MOVE_OK;
  }
  int distance = side + PITS_PER_SIDE - start_pit;
  return board->pits[start_pit] < distance ? NOT_A_FEED : MOVE_OK;
}

SowResult sow(Board *board, Side side, int start_pit) {
  MoveCheck move_check = check_sow(board, side, start_pit);
  if (!move_check) {
    return (SowResult){move_check, 0};
  }
  TakeStonesResult take_results = take_stones(board, side, start_pit);
  if (take_results.move_check) {
    for (int i = 1; i <= take_results.stones; i++) {
      put_stone(board, (start_pit + i) % N_PITS);
    }
  }
  return (SowResult){.move_check = take_results.move_check,
                     .last_pit = start_pit + take_results.stones};
}

MoveCheck check_catch(Board *board, int pit, Side side) {
  int check_pit = pit;
  bool is_grand_slam = false;
  while (is_side(side, check_pit)) {
    is_grand_slam |= TWO_OR_THREE(board->pits[check_pit]);
    check_pit = PREV_PIT(check_pit);
  }
  return is_grand_slam ? GRAND_SLAM : MOVE_OK;
}

static int catch_recursive(Board *board, int pit, Side side) {
  if (!TWO_OR_THREE(board->pits[pit]) || !is_side(side, pit)) {
    return 0;
  }
  return take_stones(board, side, pit).stones +
         catch_recursive(board, PREV_PIT(pit), side);
}

CatchResult catch(Board *board, int pit, Side side) {
  MoveCheck move_check = check_catch(board, pit, side);
  int harvest = move_check ? catch_recursive(board, pit, side) : 0;
  return (CatchResult){move_check, harvest};
}
