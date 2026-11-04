#include "board.h"
#include <stdbool.h>

bool is_side(Side side, int pit) { return side == A && pit < PITS_PER_SIDE; }

int take_stones(Board *board, int pit) {
  int stones = board->pits[pit];
  board->pits[pit] = 0;
  return stones;
}

void put_stone(Board *board, int pit) { board->pits[pit]++; }

void sow(Board *board, int start_pit) {
  int stones = take_stones(board, start_pit);

  for (int i = 1; i <= stones; i++) {
    put_stone(board, (start_pit + i) % N_PITS);
  }
}

int catch(Board *board, int pit, int side) {
  if (!TWO_OR_THREE(board->pits[pit]) || !is_side(side, pit)) {
    return 0;
  }
  return take_stones(board, pit) + catch(board, NEXT_PIT(pit), side);
}
