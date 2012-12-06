#include "hash.h"

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
