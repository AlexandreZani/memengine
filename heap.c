#include "heap.h"

#include <stdlib.h>

heap_t*
create_heap(uint32_t slab_size, uint32_t slab_num) {
  heap_t *heap = malloc(sizeof(heap_t));

  heap->slab_size = slab_size;
  heap->slab_num = slab_num;
  heap->total_size = slab_size * slab_num;
  heap->mem = malloc(heap->total_size);

  return heap;
}
