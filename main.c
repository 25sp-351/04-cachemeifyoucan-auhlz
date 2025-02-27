#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "rodcut.h"

int main(int argc, char *argv[]) {
  if (argc > 3 || argc < 2) {
    printf(
        "Error: Invalid Argument Count.\nUse this format:  %s <filename.txt> "
        "<cache.so>\n",
        argv[0]);
    return 1;
  }
  if (strstr(argv[1], ".txt") == NULL ||
      strcmp(argv[1] + strlen(argv[1]) - 4, ".txt") != 0) {
    printf("Error: First argument must be a .txt file.\n");
    return 1;
  }

  if (argc == 3 && (strstr(argv[2], ".so") == NULL ||
                    strcmp(argv[2] + strlen(argv[2]) - 3, ".so") != 0)) {
    printf("Error: Second argument must be a .so file.\n");
    return 1;
  }

  ProviderFunction get_me_a_value = rod_cutting;

  bool cache_installed = argc > 2;
  Cache *cache = NULL;

  read_piece_values(argv[1]);

  if (cache_installed) {
    cache = load_cache_module(argv[2]);

    if (cache == NULL) {
      fprintf(stderr, "Failed to load cache module\n");
      return 1;
    }
    // replace our real provider with a caching provider
    get_me_a_value = cache->set_provider_func(get_me_a_value);
  }

  int rod_length;
  printf("Enter rod lengths (one per line, CTRL+D to stop):\n");

  while (scanf("%d", &rod_length) == 1) {
    if (rod_length <= 0) {
      printf("Error: Invalid rod length. Must be a positive integer.\n");
      continue;
    }

    // Call the rod_cutting function with our Rodcut entry.
    Vec result = get_me_a_value(rod_length);

    printf("\nRod Length: %d\n", rod_length);

    print_piece_values(result, rod_length);
  }
  if (cache_installed) {
    printf("\n\n");

    CacheStat *list_of_stats = cache->get_statistics();
    print_cache_stats(fileno(stdout), list_of_stats);

    if (list_of_stats)
      free(list_of_stats);

    printf("\n\n");

    cache->cache_cleanup();
    free(cache);
  }

  return 0;
}
