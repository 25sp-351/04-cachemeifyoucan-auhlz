#include "rodcut.h"
#include "vec.h"

#include <stdio.h>
#include <string.h>

PieceLengthValue pieces[MAX_PIECES];

rod_func_ptr rod_cutting_provider;

// Calculate current value of rod
int max_rod_value(Vec v) {
  PieceLengthValue *pieces = vec_items(v);
  int total_value = 0;
  for (int ix = 0; ix < vec_length(v); ix++) {
    total_value += pieces[ix].value;
  }
  return total_value;
}

void update_best_cut_counts(Vec *best_cut_counts, int *best_value,
                            Vec current_cut_counts, int piece_value,
                            int piece_index) {
  int current_value = max_rod_value(current_cut_counts) + piece_value;

  if (current_value > *best_value) {
    *best_value = current_value;

    vec_free(*best_cut_counts);                      // Free old best solution
    *best_cut_counts = vec_copy(current_cut_counts); // Copy best so far

    int *best_cut_counts_items = vec_items(*best_cut_counts);
    best_cut_counts_items[piece_index] += 1; // Track usage of this piece
  }
}

Vec rod_cutting(int rod_length) {
  Vec best_cut_counts = new_vec(sizeof(int)); // Stores counts per piece index
  int zero = 0;
  for (int ix = 0; ix < MAX_PIECES; ix++) {
    vec_add(best_cut_counts, &zero); // Initialize counts to 0
  }

  int best_value = 0;

  if (rod_length <= 0) {
    return best_cut_counts;
  }

  for (int ix = 0; ix < MAX_PIECES; ix++) {

    if (pieces[ix].length <= rod_length && pieces[ix].value > 0) {
      Vec current_cut_counts;

      if (rod_cutting_provider) {
        current_cut_counts =
            (*rod_cutting_provider)(rod_length - pieces[ix].length);
      } else {
        current_cut_counts = rod_cutting(rod_length - pieces[ix].length);
      }

      update_best_cut_counts(&best_cut_counts, &best_value, current_cut_counts,
                             pieces[ix].value, ix);
      vec_free(current_cut_counts);
    }
  }

  return best_cut_counts;
}

// Takes a vector of counts then multiply it to find respective piece values
void print_piece_values(Vec vector, int rod_length) {
  int *counts = vec_items(vector);
  int total_value = 0;
  int total_length_used = 0;

  for (int ix = 0; ix < vec_length(vector) && ix < MAX_PIECES; ix++) {
    if (counts[ix] > 0) {
      int piece_value = pieces[ix].value * counts[ix];
      total_value += piece_value;
      total_length_used += pieces[ix].length * counts[ix];
      printf("%d @ %d = %d\n", counts[ix], pieces[ix].length, piece_value);
    }
  }

  int remainder = rod_length - total_length_used;
  printf("Total Value: %d\n", total_value);
  printf("Remainder: %d\n", remainder);
}

void read_piece_values(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error: Unable to open file %s\n", filename);
    return;
  }

  Vec val_list = new_vec(sizeof(PieceLengthValue));

  int ix = 0;
  while (ix < MAX_PIECES) {
    PieceLengthValue item;
    if (fscanf(file, "%d, %d ", &item.length, &item.value) != 2 ||
        item.length < 1 || item.value < 1) {
      break;
    }
    vec_add(val_list, &item);
    pieces[ix] = item; // Update the global pieces array
    ix++;
  }

  fclose(file);

  if (vec_length(val_list) == 0) {
    vec_free(val_list);
    return;
  }
}
