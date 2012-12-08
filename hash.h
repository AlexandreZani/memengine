#ifndef __HASH_H
#define __HASH_H
#include <stdint.h>

extern uint32_t hash_32(uint32_t *data, uint32_t len);
extern uint32_t hash_8(uint8_t *data, uint32_t len);

struct hash_table_entry_struct {
  uint32_t key_size;
  uint8_t *key;
  void *data;
  struct hash_table_entry_struct *next;
};
typedef struct hash_table_entry_struct hash_table_entry_t;

typedef struct {
  uint32_t mask;
  hash_table_entry_t *entries;
} hash_table_t;

extern hash_table_t* create_hash_table(uint32_t size);
extern void hash_table_set(hash_table_t* table, uint8_t *key,
    uint32_t key_size, void* data);
extern void* hash_table_get(hash_table_t* table, uint8_t *key,
    uint32_t key_size);

#endif
