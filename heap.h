#ifndef __HEAP_H
#define __HEAP_H

#include <stdint.h>

typedef struct {
  uint32_t chunk_size;
  void *open_field;
  void *mem;
} slab_t;

typedef struct {
  uint32_t total_size;
  uint32_t slab_size;
  uint32_t slab_num;
  void *mem;
  slab_t *slabs;
} heap_t;

extern heap_t *create_heap(uint32_t slab_size, uint32_t slab_num);

#endif
