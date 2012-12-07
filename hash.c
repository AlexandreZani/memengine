#include "hash.h"

#include <stdlib.h>
#include <string.h>

uint32_t
hash_8(uint8_t *data, uint32_t len) {
  uint32_t new_len = len;
  if (len % 4 > 0) {
    new_len += 4 - (len % 4);
  }
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
