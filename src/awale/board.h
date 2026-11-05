#ifndef BOARD_H

#define BOARD_H

#include <stdbool.h>
#define PITS_PER_SIDE 6
#define N_PITS (2 * PITS_PER_SIDE)
#define N_STONES (START_STONES_PER_PIT * N_PITS)
#define TWO_OR_THREE(x) ((x) == 2 || (x) == 3)
#define NEXT_PIT(pit) (((pit) + 1) % N_PITS)
#define PREV_PIT(pit) ((((pit) + N_PITS) - 1) % N_PITS)

typedef struct Board {
  int pits[2 * PITS_PER_SIDE];
} Board;

typedef enum { A = 0, B = 6 } Side;

Board board(int start_stones);

bool check_pit(long pit);
bool is_side(Side side, int pit);
bool side_is_empty(Board *board, Side side);
bool sides_is_empty(Board *baord, Side side);

Side other_side(Side side);

void put_stone(Board *board, int pit);
int take_stones(Board *board, int pit);

void print_board(Board *board);

#endif // !BOARD_H
