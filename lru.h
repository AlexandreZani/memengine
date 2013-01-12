#ifndef __LRU_H
#define __LRU_H

#include "cache_entry.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct {
  cache_entry_t *most_recent;
  cache_entry_t *least_recent;
  pthread_mutex_t mutex;
} lru_queue_t;

extern lru_queue_t *create_lru_queue();
extern void lru_add_new_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry);
extern bool lru_bump_used_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry);
extern cache_entry_t *lru_pop_least_recent(lru_queue_t *lru_queue);
extern bool lru_remove_entry(lru_queue_t *lru_queue, cache_entry_t * cache_entry);
extern void destroy_lru_queue(lru_queue_t *lru_queue);

#endif
