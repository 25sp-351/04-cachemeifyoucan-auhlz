#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

// Fifo cache implementation

#define MAX_KEY 5
#define CACHE_SIZE MAX_KEY + 1

// Doubly linked list node
typedef struct Node {
  KeyType key;
  ValueType value;
  struct Node *prev;
  struct Node *next;
} Node;

// Fifo Cache structure
typedef struct {
  int capacity;
  int size;
  Node *head;
  Node *tail;
} FifoCache;

int cache_requests;
int cache_hits;

FifoCache *cache = NULL;
ProviderFunction _downstream = NULL;

void initialize(void) {
  fprintf(stderr, __FILE__ " initialize()\n");

  cache_requests = 0;
  cache_hits = 0;

  cache = (FifoCache *)malloc(sizeof(FifoCache));
  cache->capacity = CACHE_SIZE;
  cache->size = 0;
  cache->head = NULL;
  cache->tail = NULL;
}

CacheStat *statistics(void) {
  fprintf(stderr, __FILE__ " statistics()\n");

  CacheStat *current_stats = malloc(3 * sizeof(CacheStat));
  current_stats[0] = (CacheStat){Cache_requests, cache_requests};
  current_stats[1] = (CacheStat){Cache_hits, cache_hits};
  current_stats[2] = (CacheStat){END_OF_STATS, 0};

  return current_stats;
}

void reset_statistics(void) {
  fprintf(stderr, __FILE__ " reset_statistics()\n");
  cache_requests = 0;
  cache_hits = 0;
}

Node *create_node(KeyType key, ValueType value) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->key = key;
  node->value = value;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

// Remove oldest front element to maintain FIFO order
void _remove_from_front(void) {
  if (!cache->head) {
    return; // Nothing to remove
  }
  Node *old_head = cache->head;
  cache->head = old_head->next;

  if (cache->head) {
    cache->head->prev = NULL;
  } else {
    cache->tail = NULL;
  }

  fprintf(stderr, __FILE__ " removed(" KEY_FMT ")\n", old_head->key);
  free(old_head);

  cache->size--;
}

// Search for a key in the cache
Node *_get(KeyType key) {
  Node *current = cache->head;
  while (current) {
    if (current->key == key) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// Insert a key-value pair at the back to maintain FIFO order
void _insert_at_back(KeyType key, ValueType value) {
  if (cache->size == cache->capacity) {
    _remove_from_front();
  }

  Node *new_node = create_node(key, value);
  if (cache->tail) {
    cache->tail->next = new_node;
    new_node->prev = cache->tail;
  }
  cache->tail = new_node;

  if (!cache->head) {
    cache->head = new_node;
  }
  fprintf(stderr, __FILE__ " insert(" KEY_FMT ")\n", key);
  cache->size++;
}

// FIFO cache provider function
ValueType _caching_provider(KeyType key) {
  cache_requests++;

  Node *node = _get(key);
  if (node) {
    cache_hits++;
    fprintf(stderr, __FILE__ " cached(" KEY_FMT ")\n", key);
    return node->value;
  }

  // If key is not in cache, fetch from downstream
  ValueType result = (*_downstream)(key);
  _insert_at_back(key, result); // Add new data at the back

  return result;
}

ProviderFunction set_provider(ProviderFunction downstream) {
  fprintf(stderr, __FILE__ " set_provider()\n");
  _downstream = downstream;
  return _caching_provider;
}
