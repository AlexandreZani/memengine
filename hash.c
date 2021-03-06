#include "hash.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint32_t
hash_8(uint8_t *data, uint32_t len) {
  if (len % 4 == 0) {
    return hash_32((uint32_t *)data, len / 4);
  }
  uint32_t new_len = len + 4 - (len % 4);
  uint8_t *new_data = malloc(new_len);
  uint32_t hash;

  memcpy(new_data, data, len);
  memset(new_data + len, 0x00, 4 - len % 4);

  hash = hash_32((uint32_t *)new_data, new_len / 4);

  if (new_data) {
    free(new_data);
    new_data = NULL;
  }

  return hash;
}

/*****************************************************************************
 * hash_32
 *
 * This is the "one-at-a-time" algorithm that can be found here:
 * http://burtleburtle.net/bob/hash/doobs.html
 * It was not selected merely for ease of implementation.
*****************************************************************************/
uint32_t
hash_32(uint32_t *data, uint32_t len) {
  uint32_t hash = 0;

  for (uint32_t i = 0; i < len; i++) {
    hash += data[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash << 11);
  hash += (hash << 15);

  return hash;
}

hash_table_t*
create_hash_table(size_t size) {
  hash_table_t *table = malloc(sizeof(hash_table_t));
  pthread_mutex_init(&(table->mutex), NULL);

  table->mask = 0x1;
  while (table->mask < size-1) {
    table->mask <<= 1;
    table->mask += 1;
  }

  table->entries = malloc(sizeof(cache_entry_t *) * (table->mask+1));
  memset(table->entries, 0x00, sizeof(cache_entry_t *) * (table->mask+1));

  return table;
}

void
destroy_hash_table(hash_table_t *hash_table) {
  if (hash_table->entries != NULL) {
    free(hash_table->entries);
    hash_table->entries = NULL;
  }
  free(hash_table);
}

cache_entry_t *
hash_table_index_entry(hash_table_t *table, cache_entry_t *entry) {
  uint32_t hash = hash_8(entry->key, entry->key_size);
  pthread_mutex_lock(&(table->mutex));
  cache_entry_t **index_entry = table->entries + (hash & table->mask);
  cache_entry_t *to_free = NULL;


  while (*index_entry != NULL && !(
        ((*index_entry)->key_size == entry->key_size) &&
        (memcmp((*index_entry)->key, entry->key, entry->key_size) == 0))) {
    index_entry = &((*index_entry)->hash_table_next);
  }

  if (*index_entry != NULL) {
    to_free = *index_entry;
    entry->hash_table_next = to_free->hash_table_next;
  }

  *index_entry = entry;
  pthread_mutex_unlock(&(table->mutex));
  return to_free;
}

cache_entry_t *
hash_table_get_entry(hash_table_t *table, uint8_t *key, size_t key_size) {
  pthread_mutex_lock(&(table->mutex));
  uint32_t hash = hash_8(key, key_size);
  cache_entry_t **entry = table->entries + (hash & table->mask);

  while ((*entry != NULL) && !(
        ((*entry)->key_size == key_size) &&
        (memcmp((*entry)->key, key, key_size) == 0))) {
    entry = &((*entry)->hash_table_next);
  }

  pthread_mutex_unlock(&(table->mutex));
  return *entry;
}

cache_entry_t *
hash_table_deindex(hash_table_t *table, uint8_t *key, size_t key_size) {
  pthread_mutex_lock(&(table->mutex));
  uint32_t hash = hash_8(key, key_size);
  cache_entry_t **entry = table->entries + (hash & table->mask);
  cache_entry_t *old_entry = NULL;

  while ((*entry != NULL) && !(
        ((*entry)->key_size == key_size) &&
        (memcmp((*entry)->key, key, key_size) == 0))) {
    entry = &((*entry)->hash_table_next);
  }

  old_entry = *entry;
  if (*entry != NULL) {
    *entry = (*entry)->hash_table_next;
  }

  pthread_mutex_unlock(&(table->mutex));
  return old_entry;
}
