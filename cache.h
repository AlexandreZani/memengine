#ifndef __CACHE_H
#define __CACHE_H

#include <stdint.h>
#include <stdlib.h>

#include "arenas.h"
#include "hash.h"
#include "lru.h"
#include "cache_entry.h"

typedef struct {
  heap_t *heap;
  hash_table_t *hash_table;
  lru_queue_t *lru_queue;
} cache_t;

extern cache_t *create_cache(size_t mem_div_size, uint32_t mem_div_num,
    size_t hash_table_size);
extern void destroy_cache(cache_t *cache);
extern void cache_set_item(cache_t *cache, uint8_t *key, size_t key_size,
    uint8_t* data, size_t data_size);
extern cache_entry_t* cache_get_item(cache_t *cache, uint8_t *key,
    size_t key_sz);
#endif
