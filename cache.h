#ifndef __CACHE_H
#define __CACHE_H

#include <stdint.h>
#include <stdlib.h>

#include "arenas.h"
#include "hash.h"
#include "lru.h"

typedef struct {
  heap_t *heap;
  hash_table_t *hash_table;
  lru_queue_t *lru_queue;
} cache_t;

extern cache_t *create_cache(size_t mem_div_size, uint32_t mem_div_num, size_t hash_table_size);

#endif
