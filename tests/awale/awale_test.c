#include "awale/awale.h"
#include <assert.h>

static void test_player_for_side(void) { /* TODO: add assertions. */ }

static void test_move(void) { /* TODO: add assertions. */ }

static void test_move_grand_slam(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Game game = new_game("Alice", "Bob");
    game.board = board(0);
    game.turn = side;
    game.player_a.stone_bag = 21;
    game.player_b.stone_bag = 21;
    Side opponent = other_side(side);
    int start_pit = side + PITS_PER_SIDE - 1;

    /* Sowing one seed would make the opponent's only pit capturable. */
    game.board.pits[side] = 4;
    game.board.pits[start_pit] = 1;
    game.board.pits[opponent] = 1;

    MoveResult result = move(&game, start_pit);

    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    assert(game.turn == opponent);
    assert(game.player_a.stone_bag == 21);
    assert(game.player_b.stone_bag == 21);
    assert(!game.game_over);
    assert(!game.has_winner);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected = pit == side ? 4 : (pit == (int)opponent ? 2 : 0);
      assert(game.board.pits[pit] == expected);
    }
  }
}

static void test_new_game(void) { /* TODO: add assertions. */ }

static void test_print_player(void) { /* TODO: add assertions. */ }

static void test_print_game(void) { /* TODO: add assertions. */ }

int main(void) {
  test_player_for_side();
  test_move();
  test_move_grand_slam();
  test_new_game();
  test_print_player();
  test_print_game();
  return 0;
}
