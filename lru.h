#ifndef __LRU_H
#define __LRU_H

#include "cache_entry.h"
#include <stdbool.h>

typedef struct {
  cache_entry_t *most_recent;
  cache_entry_t *least_recent;
} lru_queue_t;

lru_queue_t *create_lru_queue();
void lru_add_new_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry);
bool lru_bump_used_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry);
cache_entry_t *lru_pop_least_recent(lru_queue_t *lru_queue);
bool lru_remove_entry(lru_queue_t *lru_queue, cache_entry_t * cache_entry);

#endif
