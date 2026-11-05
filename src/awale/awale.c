#include "src/awale/awale.h"
#include "src/awale/board.h"
#include "src/awale/moves.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// clang-format off
#include <stdlib.h>
#include <time.h>
// clang-format on

Player *player_for_side(Game *game, Side side) {
  return side == A ? &game->player_a : &game->player_b;
}

static bool can_move(Game *game, Player *player) {
  for (size_t i = player->side; i < player->side + PITS_PER_SIDE; i++) {
    if (check_sow(&game->board, player->side, i)) {
      return true;
    }
  }
  return false;
}

static bool check_stone_bag(Player player) {
  return player.stone_bag > N_STONES / 2;
}

static bool check_end(Game *game) {
  return !can_move(game, player_for_side(game, game->turn)) ||
         check_stone_bag(game->player_a) || check_stone_bag(game->player_b);
}

static Player *winning_player(Game *game) {
  return check_stone_bag(game->player_a)   ? &game->player_a
         : check_stone_bag(game->player_b) ? &game->player_b
                                           : NULL;
}

static void collect_stones(Game *game, Player *player) {
  for (size_t i = player->side; i < player->side + PITS_PER_SIDE; i++) {
    player->stone_bag += take_stones(&game->board, i);
  }
}

static Player *winner(Game *game) {
  if (can_move(game, player_for_side(game, game->turn))) {
    // both players can move,
    // check if either on them owns more then half of the stones
    return winning_player(game);
  }

  collect_stones(game, &game->player_a);
  collect_stones(game, &game->player_b);

  // at least one player can't move
  // -> collect all stones from the board

  return winning_player(game);
}

MoveResult move(Game *game, int pit) {
  if (game->game_over) {
    return (MoveResult){GAME_OVER, 0};
  };
  Player *player = player_for_side(game, game->turn);
  MoveResult move_result = sow_and_catch(&game->board, player->side, pit);
  if (move_result.move_check) {
    player->stone_bag += move_result.obtained;
    game->turn = other_side(game->turn);
    game->game_over = check_end(game);
    Player *winning_player = winner(game);
    game->has_winner = winning_player != NULL;
    if (game->has_winner) {
      game->winner = winning_player->side;
    }
  }
  return move_result;
}

Game new_game(char *name_a, char *name_b) {
  Player player_a = (Player){name_a, A, 0};
  Player player_b = (Player){name_b, B, 0};
  Side turn = (rand() % 2 == 0) ? A : B;

  return (Game){.board = board(),
                .player_a = player_a,
                .player_b = player_b,
                .turn = turn,
                .winner = A,
                .has_winner = false,
                .game_over = false};
}
void print_player(Player *player) {
  char side_string = player->side == A ? 'A' : 'B';
  printf("Side %c, Player %s - [ %2d ]\n", side_string, player->name,
         player->stone_bag);
}

void print_game(Game *game) {
  print_board(&game->board);
  print_player(&game->player_a);
  print_player(&game->player_b);
}
