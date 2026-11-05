#include "awale/moves.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

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

/* All capture cases are mirrored so both board boundaries are exercised. */
static void test_capture_single(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    for (int seeds = 1; seeds <= 2; seeds++) {
      Board b = {0};
      b.pits[side] = 4;
      b.pits[side + 5] = 1;
      b.pits[opponent] = seeds;
      b.pits[opponent + 1] = 4;
      MoveResult result = sow_and_catch(&b, side, side + 5);
      assert(result.move_check == MOVE_OK);
      assert(result.obtained == seeds + 1);
      assert(b.pits[opponent] == 0);
      assert(b.pits[opponent + 1] == 4);
      assert(b.pits[side] == 4);
      assert(b.pits[side + 5] == 0);
    }
  }
}

static void test_capture_chain(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    Board b = {0};
    b.pits[side + 5] = 3;
    b.pits[opponent] = 1;
    b.pits[opponent + 1] = 2;
    b.pits[opponent + 2] = 1;
    b.pits[opponent + 3] = 4;
    MoveResult result = sow_and_catch(&b, side, side + 5);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 7);
    for (int pit = 0; pit < N_PITS; pit++) {
      assert(b.pits[pit] == (pit == opponent + 3 ? 4 : 0));
    }
  }
}

static void test_capture_blockers(void) {
  /* Sowing increments every opponent pit up to the final pit, so a
   * zero cannot occur inside a reachable capture chain. The reachable
   * blockers are 1 and 4; surrounding zero pits remain untouched. */
  int blockers[] = {1, 4};
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    for (size_t i = 0; i < sizeof blockers / sizeof blockers[0]; i++) {
      Board b = {0};
      b.pits[side + 5] = 2;
      b.pits[opponent] = blockers[i] - 1;
      b.pits[opponent + 1] = 1;
      b.pits[opponent + 2] = 4;
      MoveResult result = sow_and_catch(&b, side, side + 5);
      assert(result.move_check == MOVE_OK);
      assert(result.obtained == 2);
      assert(b.pits[opponent] == blockers[i]);
      assert(b.pits[opponent + 1] == 0);
      assert(b.pits[opponent + 2] == 4);
      assert(b.pits[opponent + 3] == 0);

      /* A noncapturable final pit prevents capture from starting. */
      b = (Board){0};
      b.pits[side + 5] = 1;
      b.pits[opponent] = blockers[i] - 1;
      b.pits[opponent + 1] = 4;
      result = sow_and_catch(&b, side, side + 5);
      assert(result.move_check == MOVE_OK);
      assert(result.obtained == 0);
      assert(b.pits[opponent] == blockers[i]);
      assert(b.pits[opponent + 1] == 4);
    }
  }
}

static void test_capture_stops_at_own_side(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    Board b = {0};
    b.pits[side + 4] = 2;
    b.pits[side + 5] = 1;
    b.pits[opponent] = 1;
    b.pits[opponent + 1] = 4;
    MoveResult result = sow_and_catch(&b, side, side + 4);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 2);
    assert(b.pits[side + 5] == 2);
    assert(b.pits[opponent] == 0);
    assert(b.pits[opponent + 1] == 4);
  }
}

static void test_capture_lands_on_own_side(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Board b = {0};
    b.pits[side] = 1;
    b.pits[side + 1] = 1;
    b.pits[other_side(side)] = 4;
    Board expected = b;
    expected.pits[side] = 0;
    expected.pits[side + 1] = 2;
    MoveResult result = sow_and_catch(&b, side, side);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    assert_unchanged(&b, &expected);
  }
}

static void test_grand_slam_chain(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    Board b = {0};
    b.pits[side + 5] = 3;
    b.pits[opponent] = 1;
    b.pits[opponent + 1] = 2;
    b.pits[opponent + 2] = 1;
    MoveResult result = sow_and_catch(&b, side, side + 5);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected = pit == opponent || pit == opponent + 2
                         ? 2
                         : (pit == opponent + 1 ? 3 : 0);
      assert(b.pits[pit] == expected);
    }
  }
}

static void test_sowing_wraparound(void) {
  Board b = {0};
  b.pits[11] = 3;
  MoveResult result = sow_and_catch(&b, B, 11);
  assert(result.move_check == MOVE_OK);
  assert(result.obtained == 0);
  for (int pit = 0; pit < N_PITS; pit++) {
    assert(b.pits[pit] == (pit < 3 ? 1 : 0));
  }
}

static void test_sowing_skip_origin(void) {
  /* After 11 seeds each OTHER pit has one; seed 12 goes past the origin. */
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Board b = {0};
    b.pits[side] = 12;
    MoveResult result = sow_and_catch(&b, side, side);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected = pit == side ? 0 : (pit == side + 1 ? 2 : 1);
      assert(b.pits[pit] == expected);
    }
  }
}

static void test_sowing_multiple_laps(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Board b = {0};
    b.pits[side] = 23;
    MoveResult result = sow_and_catch(&b, side, side);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected = pit == side ? 0 : (pit == side + 1 ? 3 : 2);
      assert(b.pits[pit] == expected);
    }
  }
}

static void test_sowing_capture_after_skip(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int opponent = other_side(side);
    Board b = {0};
    b.pits[side] = 17;
    /* One full lap and six more seeds end in the FIRST opponent pit.
     * This also verifies capture uses the last pit after skipping. */
    MoveResult result = sow_and_catch(&b, side, side);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 2);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected =
          pit == side || pit == opponent ? 0 : (is_side(side, pit) ? 2 : 1);
      assert(b.pits[pit] == expected);
    }
  }
}

static void test_rejected_moves(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int invalid[] = {-1, N_PITS, other_side(side), side + 1};
    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; i++) {
      Board b = board();
      b.pits[side + 1] = 0;
      Board before = b;
      MoveResult result = sow_and_catch(&b, side, invalid[i]);
      assert(!result.move_check);
      assert(result.obtained == 0);
      assert_unchanged(&b, &before);
    }
    Board b = {0};
    b.pits[side] = 1; /* Cannot reach the empty opponent side. */
    Board before = b;
    MoveResult result = sow_and_catch(&b, side, side);
    assert(!result.move_check);
    assert(result.obtained == 0);
    assert_unchanged(&b, &before);
  }
}

/* An optional case name lets Meson report failures independently. */
int main(int argc, char **argv) {
  if (argc == 1 || strcmp(argv[1], "sow_and_catch") == 0) {
    test_sow_and_catch();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "check_sow_ownership_and_stones") == 0) {
    test_check_sow_ownership_and_stones();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "check_sow_feeding") == 0) {
    test_check_sow_feeding();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_single") == 0) {
    test_capture_single();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_chain") == 0) {
    test_capture_chain();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_blockers") == 0) {
    test_capture_blockers();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_stops_at_own_side") == 0) {
    test_capture_stops_at_own_side();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_lands_on_own_side") == 0) {
    test_capture_lands_on_own_side();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "grand_slam_chain") == 0) {
    test_grand_slam_chain();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "sowing_wraparound") == 0) {
    test_sowing_wraparound();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "sowing_skip_origin") == 0) {
    test_sowing_skip_origin();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "sowing_multiple_laps") == 0) {
    test_sowing_multiple_laps();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "sowing_capture_after_skip") == 0) {
    test_sowing_capture_after_skip();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "rejected_moves") == 0) {
    test_rejected_moves();
    if (argc > 1) {
      return 0;
    }
  }
  return argc == 1 ? 0 : 1;
}
