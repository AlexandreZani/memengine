#ifndef __HEAP_H
#define __HEAP_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  size_t chunk_size;
  size_t slab_size;
  void *open_field;
  void *mem;
  void *free_chunks;
} slab_t;

typedef struct {
  uint32_t total_size;
  uint32_t slab_size;
  uint32_t slab_num;
  void *mem;
  slab_t *slabs;
} heap_t;

extern heap_t *create_heap(size_t slab_size, uint32_t slab_num);
extern void *alloc_chunk(heap_t *heap, size_t size);
extern void free_chunk(heap_t *heap, void *chunk);

#endif
