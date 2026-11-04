#ifndef BOARD_H

#define BOARD_H

#define PITS_PER_SIDE 6
#define N_PITS (2 * PITS_PER_SIDE)
#define START_STONES_PER_PIT 4
#define N_STONES (START_STONES_PER_PIT * N_PITS)
#define TWO_OR_THREE(x) ((x) == 2 || (x) == 3)
#define NEXT_PIT(pit) (((pit) + 1) % N_PITS)

typedef struct Board {
  int pits[2 * PITS_PER_SIDE];
} Board;

typedef enum { A, B } Side;

void sow(Board *board, int start_pit);

int catch (Board *board, int pit, int side);

#endif // !BOARD_H
