#include "awale/board.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>

static void test_board(void) {
  Board b = board(4);
  for (size_t i = 0; i < N_PITS; i++) {
    assert(b.pits[i] == 4);
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

static void test_is_side(void) { /* TODO: add assertions. */ }

static void test_sides_is_empty(void) { /* TODO: add assertions. */ }

static void test_other_side(void) { /* TODO: add assertions. */ }

static void test_take_stones(void) { /* TODO: add assertions. */ }

static void test_put_stone(void) { /* TODO: add assertions. */ }

static void test_print_board(void) { /* TODO: add assertions. */ }

int main(void) {
  test_board();
  test_check_pit();
  test_is_side();
  test_sides_is_empty();
  test_other_side();
  test_take_stones();
  test_put_stone();
  test_print_board();
  return 0;
}
