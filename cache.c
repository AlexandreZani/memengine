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

void
cache_set_item(cache_t *cache, uint8_t *key, size_t key_size, uint8_t *data,
    size_t data_size) {
  size_t min_entry_size = calc_cache_entry_size(key_size, data_size);
  cache_entry_t *cache_entry = alloc_chunk(cache->heap, min_entry_size);
  assemble_cache_entry(cache_entry, key, key_size, data, data_size);
  hash_table_index_entry(cache->hash_table, cache_entry);
  lru_add_new_entry(cache->lru_queue, cache_entry);
}

cache_entry_t*
cache_get_item(cache_t *cache, uint8_t *key, size_t key_sz) {
  return hash_table_get_entry(cache->hash_table, key, key_sz);
}
