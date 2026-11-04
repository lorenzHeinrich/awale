#include "board.h"
#include <assert.h>
#include <stddef.h>

static Board board(void) {
  Board board = {0};
  for (int i = 0; i < N_PITS; i++) {
    board.pits[i] = START_STONES_PER_PIT;
  }
  return board;
}

static void test_board_setup(void) {
  Board b = board();
  for (size_t i = 0; i < N_PITS; i++) {
    assert(b.pits[i] == START_STONES_PER_PIT);
  }
}

static void test_sow(void) {
  Board b = board();

  sow(&b, 0);

  assert(b.pits[0] == 0);
  for (size_t i = 1; i <= 4; i++) {
    assert(b.pits[i] == START_STONES_PER_PIT + 1);
  };
}

int main(void) {
  test_board_setup();
  test_sow();
  return 0;
}
