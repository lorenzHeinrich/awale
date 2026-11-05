#include "board.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

Board board(void) {
  Board board = {0};
  for (int i = 0; i < N_PITS; i++) {
    board.pits[i] = START_STONES_PER_PIT;
  }
  return board;
}

bool is_side(Side side, int pit) {
  int first_pit = (int)side;
  return pit >= first_pit && pit < first_pit + PITS_PER_SIDE;
}

bool sides_is_empty(Board *baord, Side side) {
  for (size_t i = side; i < side + PITS_PER_SIDE; i++) {
    if (baord->pits[i] > 0) {
      return false;
    }
  }
  return true;
}

bool check_pit(long pit) { return pit >= 0 && pit < N_PITS; }

Side other_side(Side side) { return side == A ? B : A; }

bool check_take_stones(Board *board, Side side, int pit) {
  if (!is_side(side, pit)) {
    return NOT_SIDES_PIT;
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
  if (!sides_is_empty(board, other_side(side))) {
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
                     .last_pit = (start_pit + take_results.stones) % N_PITS};
}

MoveCheck check_catch(Board *board, int pit, Side side) {
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

static void print_side(Board *board, Side side) {
  printf("|");
  for (size_t i = 0; i < PITS_PER_SIDE; i++) {
    printf(" %2d |", board->pits[side == A ? i : N_PITS - i - 1]);
  }
}

void print_board(Board *board) {
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\nB: ");
  print_side(board, B);
  printf("\n");
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\nA: ");
  print_side(board, A);
  printf("\n");
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\n");
}
