#ifndef __HASH_H
#define __HASH_H
#include <stdint.h>
#include "cache_entry.h"

extern uint32_t hash_32(uint32_t *data, uint32_t len);
extern uint32_t hash_8(uint8_t *data, uint32_t len);

typedef struct {
  uint32_t mask;
  cache_entry_t **entries;
} hash_table_t;

extern hash_table_t *create_hash_table(size_t size);
extern cache_entry_t *hash_table_index_entry(hash_table_t* table,
    cache_entry_t *entry);
extern cache_entry_t *hash_table_get_entry(hash_table_t *table, uint8_t *key,
    size_t key_size);
extern cache_entry_t *hash_table_deindex(hash_table_t *table, uint8_t *key,
    size_t key_size);
#endif
