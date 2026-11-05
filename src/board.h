#ifndef BOARD_H

#define BOARD_H

#include <stdbool.h>
#define PITS_PER_SIDE 6
#define N_PITS (2 * PITS_PER_SIDE)
#define START_STONES_PER_PIT 4
#define N_STONES (START_STONES_PER_PIT * N_PITS)
#define TWO_OR_THREE(x) ((x) == 2 || (x) == 3)
#define NEXT_PIT(pit) (((pit) + 1) % N_PITS)
#define PREV_PIT(pit) ((((pit) + N_PITS) - 1) % N_PITS)

typedef struct Board {
  int pits[2 * PITS_PER_SIDE];
} Board;

typedef enum { A = 0, B = 6 } Side;

typedef enum {
  MOVE_OK = true,
  PIT_INDEX_ERROR = false,
  PIT_EMPTY = false,
  NOT_SIDES_PIT = false,
  NOT_A_FEED = false,
  GRAND_SLAM = false,
  NOT_YOUR_TURN = false,
  GAME_OVER = false,
} MoveCheck;

typedef struct {
  MoveCheck move_check;
  int stones;
} TakeStonesResult;

typedef struct {
  MoveCheck move_check;
  int last_pit;
} SowResult;

typedef struct {
  MoveCheck move_check;
  int harvest;
} CatchResult;

Board board(void);

MoveCheck check_sow(Board *board, Side side, int start_pit);

bool check_pit(long pit);

bool check_take_stones(Board *board, Side side, int pit);

MoveCheck check_catch(Board *board, int pit, Side side);

bool side_is_empty(Board *board, Side side);

Side other_side(Side side);

SowResult sow(Board *board, Side side, int start_pit);

CatchResult catch (Board *board, int pit, Side side);

void print_board(Board *board);

#endif // !BOARD_H
