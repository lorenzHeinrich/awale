#include "awale/awale.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static void test_player_for_side(void) {
  Game game = new_game("Alice", "Bob");
  assert(player_for_side(&game, A) == &game.player_a);
  assert(player_for_side(&game, B) == &game.player_b);
  Game copy = game;
  assert(player_for_side(&copy, A) == &copy.player_a);
  assert(player_for_side(&copy, B) == &copy.player_b);
}

static void test_move(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Game game = new_game("Alice", "Bob");
    game.turn = side;
    MoveResult result = move(&game, side);
    assert(result.move_check == MOVE_OK);
    assert(result.obtained == 0);
    assert(game.turn == other_side(side));
    assert(game.player_a.stone_bag == 0);
    assert(game.player_b.stone_bag == 0);
    assert(!game.game_over);
    assert(!game.has_winner);
    for (int pit = 0; pit < N_PITS; pit++) {
      int expected = pit == side ? 0 : (pit > side && pit <= side + 4 ? 5 : 4);
      assert(game.board.pits[pit] == expected);
    }
  }
}

static void test_move_grand_slam(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Game game = new_game("Alice", "Bob");
    game.board = (Board){0};
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
    assert(game.turn == (Side)opponent);
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

static void test_new_game(void) {
  Game game = new_game("Alice", "Bob");
  assert(strcmp(game.player_a.name, "Alice") == 0);
  assert(strcmp(game.player_b.name, "Bob") == 0);
  assert(game.player_a.side == A);
  assert(game.player_b.side == B);
  assert(game.player_a.stone_bag == 0);
  assert(game.player_b.stone_bag == 0);
  assert(game.turn == A || game.turn == B);
  assert(!game.game_over);
  assert(!game.has_winner);
  for (int pit = 0; pit < N_PITS; pit++) {
    assert(game.board.pits[pit] == START_STONES_PER_PIT);
  }
}

/* Compare fields, avoiding padding bytes in the structs. */
static void assert_game_unchanged(Game *actual, Game *before) {
  for (int pit = 0; pit < N_PITS; pit++) {
    assert(actual->board.pits[pit] == before->board.pits[pit]);
  }
  assert(actual->turn == before->turn);
  assert(actual->winner == before->winner);
  assert(actual->has_winner == before->has_winner);
  assert(actual->game_over == before->game_over);
  assert(actual->player_a.name == before->player_a.name);
  assert(actual->player_b.name == before->player_b.name);
  assert(actual->player_a.side == before->player_a.side);
  assert(actual->player_b.side == before->player_b.side);
  assert(actual->player_a.stone_bag == before->player_a.stone_bag);
  assert(actual->player_b.stone_bag == before->player_b.stone_bag);
}

static void test_rejected_moves(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    int invalid[] = {-1, N_PITS, other_side(side), side + 1};
    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; i++) {
      Game game = new_game("Alice", "Bob");
      game.turn = side;
      game.board.pits[side + 1] = 0;
      Game before = game;
      MoveResult result = move(&game, invalid[i]);
      assert(!result.move_check);
      assert(result.obtained == 0);
      assert_game_unchanged(&game, &before);
    }
    Game game = new_game("Alice", "Bob");
    game.turn = side;
    game.board = (Board){0};
    game.board.pits[side] = 1;
    Game before = game;
    MoveResult result = move(&game, side);
    assert(!result.move_check);
    assert(result.obtained == 0);
    assert_game_unchanged(&game, &before);
  }
}

static void test_finished_game(void) {
  for (int has_winner = 0; has_winner <= 1; has_winner++) {
    Game game = new_game("Alice", "Bob");
    game.game_over = true;
    game.has_winner = has_winner;
    game.winner = B;
    Game before = game;
    MoveResult result = move(&game, game.turn);
    assert(!result.move_check);
    assert(result.obtained == 0);
    assert_game_unchanged(&game, &before);
  }
}

static void test_capture_scores(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    for (int seeds = 1; seeds <= 2; seeds++) {
      Game game = new_game("Alice", "Bob");
      game.turn = side;
      game.board = (Board){0};
      int opponent = other_side(side);
      game.board.pits[side] = 4;
      game.board.pits[side + 5] = 1;
      game.board.pits[opponent] = seeds;
      game.board.pits[opponent + 1] = 4;
      game.player_a.stone_bag = 19;
      game.player_b.stone_bag = 20 - seeds;
      int old_score = player_for_side(&game, side)->stone_bag;
      int other_score = player_for_side(&game, opponent)->stone_bag;
      MoveResult result = move(&game, side + 5);
      assert(result.move_check == MOVE_OK);
      assert(result.obtained == seeds + 1);
      assert(player_for_side(&game, side)->stone_bag == old_score + seeds + 1);
      assert(player_for_side(&game, opponent)->stone_bag == other_score);
      assert(game.board.pits[opponent] == 0);
      assert(game.turn == (Side)opponent);
      assert(!game.game_over);
      assert(!game.has_winner);
    }
  }
}

static void test_winning_score(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    for (int score = 22; score <= 23; score++) {
      Game game = new_game("Alice", "Bob");
      game.turn = side;
      game.board = (Board){0};
      Side opponent = other_side(side);
      game.board.pits[side] = 4;
      game.board.pits[side + 5] = 1;
      game.board.pits[opponent] = 1;
      game.board.pits[opponent + 1] = 4;
      player_for_side(&game, side)->stone_bag = score;
      player_for_side(&game, opponent)->stone_bag = 38 - score;
      MoveResult result = move(&game, side + 5);
      assert(result.move_check == MOVE_OK);
      assert(result.obtained == 2);
      assert(player_for_side(&game, side)->stone_bag == score + 2);
      /* 24 is not a win; 25 is. The opponent can still move. */
      assert(game.game_over == (score + 2 > N_STONES / 2));
      assert(game.has_winner == game.game_over);
      if (game.has_winner) {
        assert(game.winner == (Side)side);
      }
    }
  }
}

static void test_no_feed_winner(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Game game = new_game("Alice", "Bob");
    int opponent = other_side(side);
    game.turn = side;
    game.board = (Board){0};
    game.board.pits[side + 5] = 1;
    player_for_side(&game, side)->stone_bag = 23;
    player_for_side(&game, opponent)->stone_bag = 24;
    /* The next player has one seed, too far away to feed back.
     * That remaining seed belongs in their final score. */
    MoveResult result = move(&game, side + 5);
    assert(result.move_check == MOVE_OK);
    assert(game.game_over);
    assert(game.has_winner);
    assert(game.winner == (Side)opponent);
    assert(player_for_side(&game, opponent)->stone_bag == 25);
    assert(player_for_side(&game, side)->stone_bag == 23);
    for (int pit = 0; pit < N_PITS; pit++) {
      assert(game.board.pits[pit] == 0);
    }
  }
}

static void test_no_feed_draw(void) {
  for (int side = A; side <= B; side += PITS_PER_SIDE) {
    Game game = new_game("Alice", "Bob");
    int opponent = other_side(side);
    game.turn = side;
    game.board = (Board){0};
    game.board.pits[side + 5] = 1;
    player_for_side(&game, side)->stone_bag = 24;
    player_for_side(&game, opponent)->stone_bag = 23;
    MoveResult result = move(&game, side + 5);
    assert(result.move_check == MOVE_OK);
    assert(game.game_over);
    assert(!game.has_winner);
    assert(game.player_a.stone_bag == 24);
    assert(game.player_b.stone_bag == 24);
    for (int pit = 0; pit < N_PITS; pit++) {
      assert(game.board.pits[pit] == 0);
    }
  }
}

/* An optional case name lets Meson report failures independently. */
int main(int argc, char **argv) {
  if (argc == 1 || strcmp(argv[1], "player_for_side") == 0) {
    test_player_for_side();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "move") == 0) {
    test_move();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "move_grand_slam") == 0) {
    test_move_grand_slam();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "new_game") == 0) {
    test_new_game();
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
  if (argc == 1 || strcmp(argv[1], "finished_game") == 0) {
    test_finished_game();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "capture_scores") == 0) {
    test_capture_scores();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "winning_score") == 0) {
    test_winning_score();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "no_feed_winner") == 0) {
    test_no_feed_winner();
    if (argc > 1) {
      return 0;
    }
  }
  if (argc == 1 || strcmp(argv[1], "no_feed_draw") == 0) {
    test_no_feed_draw();
    if (argc > 1) {
      return 0;
    }
  }
  return argc == 1 ? 0 : 1;
}
