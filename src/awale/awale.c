#include "src/awale/awale.h"
#include "src/awale/moves.h"
#include <limits.h>
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

static Player *winner(Game *game) {
  bool player_a =
      check_stone_bag(game->player_a) || !can_move(game, &game->player_b);
  bool player_b =
      check_stone_bag(game->player_b) || !can_move(game, &game->player_a);
  if (player_a && !player_b) {
    return &game->player_a;
  }
  if (!player_a && player_b) {
    return &game->player_b;
  }
  return NULL;
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
