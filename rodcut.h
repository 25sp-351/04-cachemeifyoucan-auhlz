#ifndef RODCUT_H
#define RODCUT_H

#include "vec.h"

typedef int PieceLength;
typedef int PieceValue;

#define MAX_PIECES 10

typedef struct {
  PieceLength length;
  PieceValue value;
} PieceLengthValue;

typedef Vec (*rod_func_ptr)(int param);

Vec rod_cutting(int entry);

void update_best_cut_counts(Vec *best_cut_counts, int *best_value,
                            Vec current_cut_counts, int piece_value,
                            int piece_index);

void print_piece_values(Vec result, int rod_length);

void read_piece_values(const char *filename);

#endif
