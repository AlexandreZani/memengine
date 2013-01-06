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
  if (cache_entry == lru_queue->most_recent) {
    return true;
  }
  cache_entry->lru_more_recent->lru_less_recent = cache_entry->lru_less_recent;
  if (cache_entry->lru_less_recent != NULL) {
    cache_entry->lru_less_recent->lru_more_recent = cache_entry->lru_more_recent;
  }

  // If we are bumping forward the least recent entry, set the second-least
  // recent entry as least recent
  if (cache_entry == lru_queue->least_recent) {
    lru_queue->least_recent = cache_entry->lru_more_recent;
  }

  cache_entry->lru_less_recent = lru_queue->most_recent;
  cache_entry->lru_more_recent = NULL;
  lru_queue->most_recent->lru_more_recent = cache_entry;
  lru_queue->most_recent = cache_entry;
  return true;
}

cache_entry_t*
lru_pop_least_recent(lru_queue_t *lru_queue) {
  cache_entry_t *ret = lru_queue->least_recent;
  if (ret == NULL) {
    return ret;
  }

  if (lru_queue->most_recent == lru_queue->least_recent) {
    lru_queue->most_recent = NULL;
    lru_queue->least_recent = NULL;
  } else {
    lru_queue->least_recent = ret->lru_more_recent;
    lru_queue->least_recent->lru_less_recent = NULL;
  }
  ret->lru_less_recent = NULL;
  ret->lru_more_recent = NULL;
  return ret;
}

bool
lru_remove_entry(lru_queue_t *lru_queue, cache_entry_t * cache_entry) {
  // If the entry does not point to a less recent entry and it is not itself
  // the least recent entry this can only mean that it is not part of the lru
  // queue.
  if (cache_entry->lru_less_recent == NULL &&
      lru_queue->least_recent != cache_entry) {
    return false;
  }

  if (cache_entry == lru_queue->most_recent) {
    lru_queue->most_recent = cache_entry->lru_less_recent;
  } else {
    cache_entry->lru_more_recent->lru_less_recent = cache_entry->lru_less_recent;
  }
  if (cache_entry == lru_queue->least_recent) {
    lru_queue->least_recent = cache_entry->lru_more_recent;
  } else {
    cache_entry->lru_less_recent->lru_more_recent = cache_entry->lru_more_recent;
  }
  cache_entry->lru_less_recent = NULL;
  cache_entry->lru_more_recent = NULL;
  return true;
}
