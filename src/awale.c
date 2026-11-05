#include "src/board.h"
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// clang-format off
#include <readline/readline.h>
#include <stdlib.h>
#include <time.h>
// clang-format on

#define PLAYER_NAME_MAX_LENGTH 31

typedef struct Player {
  char *name;
  Side side;
  int stone_bag;
} Player;

typedef struct Game {
  Board board;
  Player A;
  Player B;
  Side next_turn;
  Player *winner;
  bool game_over;
} Game;

static Player *player_from_turn(Game *game) {
  return game->A.side == game->next_turn ? &game->A : &game->B;
}

bool can_move(Game *game, Player *player) {
  for (size_t i = player->side; i < player->side + PITS_PER_SIDE; i++) {
    if (check_sow(&game->board, player->side, i)) {
      return true;
    }
  }
  return false;
}

bool check_stone_bag(Player player) { return player.stone_bag > N_STONES / 2; }

bool check_end(Game *game) {
  return !can_move(game, player_from_turn(game)) || check_stone_bag(game->A) ||
         check_stone_bag(game->B);
}

Player *winner(Game *game) {
  bool player_a = check_stone_bag(game->A) || !can_move(game, &game->B);
  bool player_b = check_stone_bag(game->B) || !can_move(game, &game->A);
  if (player_a && !player_b) {
    return &game->A;
  }
  if (!player_a && player_b) {
    return &game->B;
  }
  return NULL;
}

MoveCheck move(Game *game, int pit) {
  if (game->game_over) {
    return GAME_OVER;
  }
  Player *player = player_from_turn(game);
  if (!check_pit(pit)) {
    return PIT_INDEX_ERROR;
  }
  SowResult sow_res = sow(&game->board, player->side, pit);
  if (!sow_res.move_check) {
    return sow_res.move_check;
  }
  CatchResult catch_res =
      catch(&game->board, sow_res.last_pit, other_side(player->side));
  if (!catch_res.move_check) {
    return catch_res.move_check;
  }
  player->stone_bag += catch_res.harvest;
  game->next_turn = other_side(player->side);
  game->game_over = check_end(game);
  game->winner = winner(game);
  return MOVE_OK;
}

Game new_game(char *name_a, char *name_b) {
  Side side_a = (rand() % 2 == 0) ? A : B;
  Side side_b = other_side(side_a);

  return (Game){
      .board = board(),
      .A = (Player){name_a, side_a, 0},
      .B = (Player){name_b, side_b, 0},
      .next_turn = A,
      .winner = NULL,
      .game_over = false,
  };
}

char *get_player_name(char *prompt) {
  char *name;
  while (true) {
    name = readline(prompt);
    size_t name_len = strlen(name);
    if (name_len == 0) {
      printf("Name cannot be empty\n");
      continue;
    }
    if (name_len > PLAYER_NAME_MAX_LENGTH) {
      printf("Name is too long, max length is %d\n", PLAYER_NAME_MAX_LENGTH);
      continue;
    }
    return name;
  }
}

int promt_move(Player *player) {
  char prompt[64];
  snprintf(prompt, sizeof prompt, "Move Player %s> ", player->name);

  long move;
  while (true) {
    char *resp = readline(prompt);

    errno = 0;
    char *end;
    move = strtol(resp, &end, 10);
    if (end == resp || *end != '\0') {
      printf("Invalid number input\n");
      continue;
    }
    if (errno == ERANGE || move < INT_MIN || move > INT_MAX) {
      printf("Number out of int range\n");
      continue;
    }
    return (int)move;
  }
}

void print_player(Player *player) {
  char side_string = player->side == A ? 'A' : 'B';
  printf("Side %c, Player %s - [ %2d ]\n", side_string, player->name,
         player->stone_bag);
}

void print_game(Game *game) {
  print_board(&game->board);
  print_player(&game->A);
  print_player(&game->B);
}

int main(void) {
  srand((unsigned int)time(NULL));
  char *name_a = get_player_name("Name Player A> ");
  char *name_b = get_player_name("Name Player B> ");

  Game game = new_game(name_a, name_b);

  printf("\n=== New Game Start ===\n\n");

  while (!game.game_over) {
    print_game(&game);
    while (true) {
      int pit = promt_move(player_from_turn(&game));
      MoveCheck move_check = move(&game, pit);
      if (move_check) {
        break;
      }
      printf("Invalid Move\n");
    }
    printf("Move Success\n");
  }
  printf("End of Game!! - ");
  if (game.winner) {
    printf("winner: %s\n", game.winner->name);
  } else {
    printf("draw");
  }
  return 0;
}
