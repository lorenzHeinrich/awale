#ifndef AWALE_H

#define AWALE_H

#include "src/awale/board.h"
#include "src/awale/moves.h"

#define START_STONES_PER_PIT 4
#define PLAYER_NAME_MAX_LENGTH 31

typedef struct Player {
  char *name;
  Side side;
  int stone_bag;
} Player;

typedef struct Game {
  Board board;
  Player player_a;
  Player player_b;
  Side turn;
  Side winner;
  bool has_winner;
  bool game_over;
} Game;

Player *player_for_side(Game *game, Side side);

MoveResult move(Game *game, int pit);
Game new_game(char *name_a, char *name_b);
void print_game(Game *game);
void print_player(Player *player);
#endif // !AWALE_H
