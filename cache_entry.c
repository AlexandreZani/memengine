#include "cache_entry.h"

#include <string.h>

void
assemble_cache_entry(cache_entry_t *cache_entry,
    void *key, size_t key_size, void *data, size_t data_size) {
  cache_entry->key_size = key_size;
  cache_entry->data_size = data_size;
  cache_entry->hash_table_next = NULL;

  cache_entry->key = (void *)cache_entry + sizeof(cache_entry_t);
  memcpy(cache_entry->key, key, key_size);

  cache_entry->data = cache_entry->key + key_size;
  memcpy(cache_entry->data, data, data_size);
}
