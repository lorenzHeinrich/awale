#include "awale/board.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>

static void test_board(void) {
  Board b = board();
  for (int pit = 0; pit < N_PITS; pit++) {
    assert(b.pits[pit] == START_STONES_PER_PIT);
  }
}

static void test_check_pit(void) {
  assert(!check_pit(LONG_MIN));
  assert(!check_pit(-1));
  assert(check_pit(0));
  assert(check_pit(PITS_PER_SIDE - 1));
  assert(check_pit(PITS_PER_SIDE));
  assert(check_pit(N_PITS - 1));
  assert(!check_pit(N_PITS));
  assert(!check_pit(LONG_MAX));
}

static void test_is_side(void) {
  for (int pit = 0; pit < N_PITS; pit++) {
    assert(is_side(A, pit) == (pit < PITS_PER_SIDE));
    assert(is_side(B, pit) == (pit >= PITS_PER_SIDE));
  }
  assert(!is_side(A, -1));
  assert(!is_side(B, -1));
  assert(!is_side(A, N_PITS));
  assert(!is_side(B, N_PITS));
  assert(!is_side(A, INT_MIN));
  assert(!is_side(B, INT_MIN));
  assert(!is_side(A, INT_MAX));
  assert(!is_side(B, INT_MAX));
}

static void test_sides_is_empty(void) {
  Board b = {0};
  assert(sides_is_empty(&b, A));
  assert(sides_is_empty(&b, B));

  /* A single occupied pit anywhere makes only its own side nonempty. */
  for (int pit = 0; pit < N_PITS; pit++) {
    b.pits[pit] = 1;
    assert(sides_is_empty(&b, A) == (pit >= PITS_PER_SIDE));
    assert(sides_is_empty(&b, B) == (pit < PITS_PER_SIDE));
    assert(b.pits[pit] == 1);
    b.pits[pit] = 0;
  }

  b = board();
  assert(!sides_is_empty(&b, A));
  assert(!sides_is_empty(&b, B));
}

static void test_other_side(void) {
  assert(other_side(A) == B);
  assert(other_side(B) == A);
}

static void test_take_stones(void) {
  for (int pit = 0; pit < N_PITS; pit++) {
    Board b = board();
    assert(take_stones(&b, pit) == START_STONES_PER_PIT);
    assert(b.pits[pit] == 0);
    assert(take_stones(&b, pit) == 0);
    for (int other = 0; other < N_PITS; other++) {
      assert(b.pits[other] == (other == pit ? 0 : START_STONES_PER_PIT));
    }
  }
}

static void test_put_stone(void) {
  int starting_stones[] = {0, START_STONES_PER_PIT};
  for (size_t i = 0; i < sizeof starting_stones / sizeof starting_stones[0];
       i++) {
    for (int pit = 0; pit < N_PITS; pit++) {
      Board b = {0};
      for (int other = 0; other < N_PITS; other++) {
        b.pits[other] = starting_stones[i];
      }
      put_stone(&b, pit);
      assert(b.pits[pit] == starting_stones[i] + 1);
      put_stone(&b, pit);
      for (int other = 0; other < N_PITS; other++) {
        int expected = starting_stones[i] + (other == pit ? 2 : 0);
        assert(b.pits[other] == expected);
      }
    }
  }
}

int main(void) {
  test_board();
  test_check_pit();
  test_is_side();
  test_sides_is_empty();
  test_other_side();
  test_take_stones();
  test_put_stone();
  return 0;
}
