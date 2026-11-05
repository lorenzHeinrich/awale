#include "awale/moves.h"
#include <assert.h>
#include <stddef.h>

/* Each check must leave the board unchanged. */
static void assert_unchanged(Board *actual, Board *before) {
  for (int i = 0; i < N_PITS; i++) {
    assert(actual->pits[i] == before->pits[i]);
  }
}

static void test_sow_stones(void) {
  Board b = board(4);

  _sow(&b, 0);

  assert(b.pits[0] == 0);
  for (size_t i = 1; i <= 4; i++) {
    assert(b.pits[i] == 4 + 1);
  };
}

static void test_check_sow_ownership_and_stones(void) {
  Board b = board(4);
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

static void test_check_catch(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Board b = {0};
    Board before = b;
    assert(_check_catch(&b, side, side)); // Empty side is no capture.
    assert_unchanged(&b, &before);

    b.pits[side] = 2;
    before = b;
    assert(!_check_catch(&b, side, side)); // Last opponent stones.
    assert(_check_catch(&b, other_side(side), side));
    assert_unchanged(&b, &before);

    b.pits[side + 1] = 1; // Stones outside the capture remain.
    before = b;
    assert(_check_catch(&b, side, side));
    assert_unchanged(&b, &before);

    b = (Board){0};
    for (int i = side; i < side + PITS_PER_SIDE; i++) {
      b.pits[i] = (i - side) % 2 == 0 ? 2 : 3;
    }
    before = b;
    assert(!_check_catch(&b, side + PITS_PER_SIDE - 1, side));
    assert(_check_catch(&b, side, side));
    assert_unchanged(&b, &before);

    /* Capture must stop at 0, 1, or 4, even with 2/3 behind it. */
    int blockers[] = {0, 1, 4};
    for (size_t i = 0; i < sizeof blockers / sizeof blockers[0]; i++) {
      b = (Board){0};
      b.pits[side] = 2;
      b.pits[side + 1] = blockers[i];
      b.pits[side + 2] = 3;
      before = b;
      assert(_check_catch(&b, side + 2, side));
      assert(_check_catch(&b, side + 1, side));
      assert_unchanged(&b, &before);
    }
  }
}

static void test_sow_and_catch(void) { /* TODO: add assertions. */ }

static void test_catch(void) { /* TODO: add assertions. */ }

int main(void) {
  test_sow_stones();
  test_check_sow_ownership_and_stones();
  test_check_sow_feeding();
  test_check_catch();
  test_sow_and_catch();
  test_catch();
  return 0;
}
