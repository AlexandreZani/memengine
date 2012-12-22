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
create_hash_table(uint32_t size) {
  hash_table_t *table = malloc(sizeof(hash_table_t));

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
hash_table_index_entry(hash_table_t *table, cache_entry_t *entry) {
  uint32_t hash = hash_8(entry->key, entry->key_size);
  cache_entry_t **index_entry = table->entries + (hash & table->mask);

  while (*index_entry != NULL) {
    index_entry = &((*index_entry)->hash_table_next);
  }

  *index_entry = entry;
}

cache_entry_t *
hash_table_get_entry(hash_table_t *table, uint8_t *key, size_t key_size) {
  uint32_t hash = hash_8(key, key_size);
  cache_entry_t **entry = table->entries + (hash & table->mask);

  while ((*entry != NULL) && !(
        ((*entry)->key_size == key_size) &&
        (memcmp((*entry)->key, key, key_size) == 0))) {
    entry = &((*entry)->hash_table_next);
  }

  return *entry;
}

/*
void
hash_table_set(hash_table_t* table, uint8_t *key, uint32_t key_size, void* data) {
  uint32_t hash = hash_8(key, key_size);
  hash_table_entry_t *entry = table->entries + (hash & table->mask);

  if (entry->key_size > 0) {
    if (entry->key_size == key_size &&
        memcmp(key, entry->key, key_size) == 0) {
      // When we update, we do not free the memory for the old data.
      entry->data = data;
      return;
    }

    while (entry->next != NULL) {
      if (entry->key_size == key_size &&
          memcmp(key, entry->key, key_size) == 0) {
        // When we update, we do not free the memory for the old data.
        entry->data = data;
        return;
      }
      entry = entry->next;
    }
    entry->next = malloc(sizeof(hash_table_entry_t));
    entry = entry->next;
  }

  entry->key_size = key_size;
  entry->key = malloc(key_size);
  entry->next = NULL;
  memcpy(entry->key, key, key_size);
  entry->data = data;
}

void
hash_table_unset(hash_table_t* table, uint8_t *key, uint32_t key_size) {
  uint32_t hash = hash_8(key, key_size);
  hash_table_entry_t *entry = table->entries + (hash & table->mask);
  hash_table_entry_t *prev;

  if (entry->key_size == 0) {
    return;
  }

  if (entry->key_size == key_size &&
      memcmp(key, entry->key, key_size) == 0) {
    free(entry->key);
    memset(entry, 0x00, sizeof(hash_table_entry_t));
    return;
  }

  while (
      entry != NULL &&
      entry->key_size != key_size &&
      memcmp(key, entry->key, key_size) != 0) {
    prev = entry;
    entry = entry->next;
  }

  if (entry == NULL) {
    return;
  }

  prev->next = entry->next;
  free(entry->key);
  memset(entry, 0x00, sizeof(hash_table_entry_t));
}

void*
hash_table_get(hash_table_t* table, uint8_t *key, uint32_t key_size) {
  uint32_t hash = hash_8(key, key_size);
  hash_table_entry_t *entry = table->entries + (hash & table->mask);

  if (entry->key_size == 0) {
    return NULL;
  }

  while (
      entry != NULL &&
      entry->key_size != key_size &&
      memcmp(key, entry->key, key_size) != 0) {
    entry = entry->next;
  }

  if (entry == NULL) {
    return NULL;
  }

  return entry->data;
}
*/
