#include "awale/moves.h"
#include <assert.h>
#include <stddef.h>

/* Each check must leave the board unchanged. */
static void assert_unchanged(Board *actual, Board *before) {
  for (int i = 0; i < N_PITS; i++) {
    assert(actual->pits[i] == before->pits[i]);
  }
}

static void test_sow_and_catch(void) {
  Board b = board();

  MoveResult result = sow_and_catch(&b, A, 0);

  assert(result.move_check == MOVE_OK);
  assert(result.obtained == 0);

  assert(b.pits[0] == 0);
  for (size_t i = 1; i <= 4; i++) {
    assert(b.pits[i] == 4 + 1);
  };
}

static void test_check_sow_ownership_and_stones(void) {
  Board b = board();
  Board before = b;
  assert(check_sow(&b, A, 0));
  assert(check_sow(&b, A, PITS_PER_SIDE - 1));
  assert(check_sow(&b, B, B));
  assert(check_sow(&b, B, N_PITS - 1));
  assert(!check_sow(&b, A, B));
  assert(!check_sow(&b, B, PITS_PER_SIDE - 1));
  assert(!check_sow(&b, A, -1));
  assert(!check_sow(&b, B, N_PITS));
  assert_unchanged(&b, &before);

  b.pits[0] = 0;
  b.pits[N_PITS - 1] = 0;
  before = b;
  assert(!check_sow(&b, A, 0));
  assert(!check_sow(&b, B, N_PITS - 1));
  assert_unchanged(&b, &before);
}

static void test_check_sow_feeding(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    for (int pit = side; pit < side + PITS_PER_SIDE; pit++) {
      int distance = side + PITS_PER_SIDE - pit;
      Board b = {0};
      Board before;
      if (distance > 1) {
        b.pits[pit] = distance - 1;
        before = b;
        assert(!check_sow(&b, side, pit));
        assert_unchanged(&b, &before);
      }
      b.pits[pit] = distance;
      before = b;
      assert(check_sow(&b, side, pit));
      assert_unchanged(&b, &before);

      b.pits[pit] = distance + 1;
      before = b;
      assert(check_sow(&b, side, pit));
      assert_unchanged(&b, &before);

      /* If the opponent has a stone, feeding is unnecessary. */
      b.pits[pit] = 1;
      b.pits[other_side(side)] = 1;
      before = b;
      assert(check_sow(&b, side, pit));
      assert_unchanged(&b, &before);
    }
  }
}

int main(void) {
  test_sow_and_catch();
  test_check_sow_ownership_and_stones();
  test_check_sow_feeding();
  return 0;
}
