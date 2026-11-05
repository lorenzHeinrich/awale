
#include "src/awale/awale.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>

// clangd-format-off
#include <readline/readline.h>
#include <stdlib.h>
#include <time.h>
// clangd-format-on

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
int main(void) {
  srand((unsigned int)time(NULL));
  char *name_a = get_player_name("Name Player A> ");
  char *name_b = get_player_name("Name Player B> ");

  Game game = new_game(name_a, name_b);

  printf("\n=== New Game Start ===\n\n");

  while (!game.game_over) {
    print_game(&game);
    while (true) {
      int pit = promt_move(player_for_side(&game, game.turn));
      MoveResult move_res = move(&game, pit);
      if (move_res.move_check) {
        printf("Move Success\n");
        break;
      }
      printf("Invalid Move\n");
    }
  }
  printf("End of Game!! - ");
  if (game.has_winner) {
    printf("winner: %s\n", player_for_side(&game, game.winner)->name);
  } else {
    printf("draw");
  }
  return 0;
}
