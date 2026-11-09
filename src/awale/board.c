#include "board.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

Board board(void) {
  Board board = {0};
  for (int i = 0; i < N_PITS; i++) {
    board.pits[i] = START_STONES_PER_PIT;
  }
  return board;
}

bool is_side(Side side, int pit) {
  int first_pit = (int)side;
  return pit >= first_pit && pit < first_pit + PITS_PER_SIDE;
}

bool sides_is_empty(Board *baord, Side side) {
  for (size_t i = side; i < side + PITS_PER_SIDE; i++) {
    if (baord->pits[i] > 0) {
      return false;
    }
  }
  return true;
}

bool check_pit(long pit) { return pit >= 0 && pit < N_PITS; }

Side other_side(Side side) { return side == A ? B : A; }

int take_stones(Board *board, int pit) {
  int stones = board->pits[pit];
  board->pits[pit] = 0;
  return stones;
}

void put_stone(Board *board, int pit) { board->pits[pit]++; }

static void print_side(Board *board, Side side) {
  printf("|");
  for (size_t i = 0; i < PITS_PER_SIDE; i++) {
    printf(" %2d |", board->pits[side == A ? i : N_PITS - i - 1]);
  }
}

int board_to_str(const Board *board, char *buf, size_t len) {
  size_t off = 0;
  int n;
  n = snprintf(buf + off, len - off, "   +");
  off += n < 0 ? 0 : (size_t)n;
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    n = snprintf(buf + off, off < len ? len - off : 0, "----+");
    off += n < 0 ? 0 : (size_t)n;
  }
  n = snprintf(buf + off, off < len ? len - off : 0, "\nB: |");
  off += n < 0 ? 0 : (size_t)n;
  for (size_t i = 0; i < PITS_PER_SIDE; i++) {
    n = snprintf(buf + off, off < len ? len - off : 0, " %2d |",
                 board->pits[N_PITS - i - 1]);
    off += n < 0 ? 0 : (size_t)n;
  }
  n = snprintf(buf + off, off < len ? len - off : 0, "\n   +");
  off += n < 0 ? 0 : (size_t)n;
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    n = snprintf(buf + off, off < len ? len - off : 0, "----+");
    off += n < 0 ? 0 : (size_t)n;
  }
  n = snprintf(buf + off, off < len ? len - off : 0, "\nA: |");
  off += n < 0 ? 0 : (size_t)n;
  for (size_t i = 0; i < PITS_PER_SIDE; i++) {
    n = snprintf(buf + off, off < len ? len - off : 0, " %2d |",
                 board->pits[i]);
    off += n < 0 ? 0 : (size_t)n;
  }
  n = snprintf(buf + off, off < len ? len - off : 0, "\n   +");
  off += n < 0 ? 0 : (size_t)n;
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    n = snprintf(buf + off, off < len ? len - off : 0, "----+");
    off += n < 0 ? 0 : (size_t)n;
  }
  n = snprintf(buf + off, off < len ? len - off : 0, "\n");
  off += n < 0 ? 0 : (size_t)n;
  return (int)off;
}

void print_board(Board *board) {
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\nB: ");
  print_side(board, B);
  printf("\n");
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\nA: ");
  print_side(board, A);
  printf("\n");
  printf("   +");
  for (int i = 0; i < PITS_PER_SIDE; i++) {
    printf("----+");
  }
  printf("\n");
}
