#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

// least recently used cache implementation

#define MAX_KEY 5
#define CACHE_SIZE MAX_KEY + 1

// Doubly linked list node
typedef struct Node {
  KeyType key;
  ValueType value;
  struct Node *prev;
  struct Node *next;
} Node;

// LRU Cache structure
typedef struct {
  int capacity;
  int size;
  Node *head;
  Node *tail;
} LRUCache;

int cache_requests;
int cache_hits;

LRUCache *cache = NULL;
ProviderFunction _downstream = NULL;

void initialize(void) {
  fprintf(stderr, __FILE__ " initialize()\n");

  cache_requests = 0;
  cache_hits = 0;

  cache = (LRUCache *)malloc(sizeof(LRUCache));
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
// Move a node to the front (most recently used)
void _move_to_front(Node *node) {
  if (node == cache->head) {
    return; // Already at front
  }
  // Remove node from its current position
  if (node->prev) {
    node->prev->next = node->next;
  }
  if (node->next) {
    node->next->prev = node->prev;
  }
  if (node == cache->tail) {
    cache->tail = node->prev;
  }

  // Insert node at the front
  node->next = cache->head;
  node->prev = NULL;
  if (cache->head) {
    cache->head->prev = node;
  }
  cache->head = node;

  if (!cache->tail) {
    cache->tail = node; // If list was empty
  }
}
void _remove_from_back(void) {
  // If list is empty, nothing to remove
  if (!cache->tail) {
    return;
  }
  Node *old_tail = cache->tail;

  // Update tail to previous node
  if (old_tail->prev) {
    old_tail->prev->next = NULL;
  }
  cache->tail = old_tail->prev;

  // If list is empty, update head
  if (!cache->tail) {
    cache->head = NULL;
  }
  fprintf(stderr, __FILE__ " removed(" KEY_FMT ")\n", old_tail->key);
  free(old_tail);
  cache->size--;
}
// Search for a key in the cache
Node *_get(KeyType key) {
  Node *current = cache->head;
  while (current) {
    if (current->key == key)
      return current;
    current = current->next;
  }
  return NULL;
}

// Insert a key-value pair at the front
void _insert_at_front(KeyType key, ValueType value) {
  Node *existing_node = _get(key);
  fprintf(stderr, __FILE__ " insert(" KEY_FMT ")\n", key);

  if (existing_node) {
    // Update value and move to front
    existing_node->value = value;
    _move_to_front(existing_node);
  } else {
    // Insert new node at front
    if (cache->size == cache->capacity) {
      _remove_from_back();
    }

    Node *new_node = create_node(key, value);
    new_node->next = cache->head;
    if (cache->head) {
      cache->head->prev = new_node;
    }
    cache->head = new_node;
    // Update tail if list was empty
    if (!cache->tail) {
      cache->tail = new_node;
    }
    cache->size++;
  }
}

// LRU cache provider function
ValueType _caching_provider(KeyType key) {
  cache_requests++;

  Node *node = _get(key);
  if (node) {
    cache_hits++;
    _move_to_front(node);
    fprintf(stderr, __FILE__ " cached(" KEY_FMT ")\n", key);
    return node->value;
  }

  // If key is not in cache, get value from downstream
  ValueType result = (*_downstream)(key);
  _insert_at_front(key, result);

  return result;
}

ProviderFunction set_provider(ProviderFunction downstream) {
  fprintf(stderr, __FILE__ " set_provider()\n");
  _downstream = downstream;
  return _caching_provider;
}
