#include "cache.h"

cache_t*
create_cache(size_t mem_div_size, uint32_t mem_div_num,
    size_t hash_table_size) {
  cache_t *cache = malloc(sizeof(cache_t));

  cache->heap = create_heap(mem_div_size, mem_div_num);
  cache->hash_table = create_hash_table(hash_table_size);
  cache->lru_queue = create_lru_queue();
  return cache;
}

void
destroy_cache(cache_t *cache) {
  destroy_hash_table(cache->hash_table);
  cache->hash_table = NULL;
  destroy_lru_queue(cache->lru_queue);
  cache->lru_queue = NULL;
  destroy_heap(cache->heap);
  cache->heap = NULL;
}
