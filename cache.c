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
