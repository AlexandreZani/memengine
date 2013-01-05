#include "lru.h"

lru_queue_t *
create_lru_queue() {
  lru_queue_t *lru_queue = malloc(sizeof(lru_queue_t));
  lru_queue->most_recent = NULL;
  lru_queue->least_recent = NULL;
  return lru_queue;
}

void
lru_add_new_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry) {
  if (lru_queue->most_recent == NULL) {
    lru_queue->most_recent = cache_entry;
    lru_queue->least_recent = cache_entry;
    cache_entry->lru_more_recent = NULL;
    cache_entry->lru_less_recent = NULL;
    return;
  }

  lru_queue->most_recent->lru_more_recent = cache_entry;
  cache_entry->lru_less_recent = lru_queue->most_recent;
  cache_entry->lru_more_recent = NULL;
  lru_queue->most_recent = cache_entry;
}

bool
lru_bump_used_entry(lru_queue_t *lru_queue, cache_entry_t *cache_entry) {
  cache_entry->lru_more_recent->lru_less_recent = cache_entry->lru_less_recent;
  cache_entry->lru_less_recent->lru_more_recent = cache_entry->lru_more_recent;
  cache_entry->lru_less_recent = lru_queue->most_recent;
  lru_queue->most_recent = cache_entry;
  return true;
}
