#ifndef __CACHE_ENTRY_H
#define __CACHE_ENTRY_H

#include <stdlib.h>

typedef struct {
  size_t key_size;
  size_t data_size;
  void *key;
  void *data;
} cache_entry_t;

#define calc_cache_entry_size(key_size, data_size) \
  (sizeof(cache_entry_t) + key_size + data_size)

extern void assemble_cache_entry(cache_entry_t *cache_entry,
    void *key, size_t key_size, void *data, size_t data_size);

#endif
