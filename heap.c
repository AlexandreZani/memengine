#include "heap.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

heap_t*
create_heap(size_t slab_size, uint32_t slab_num) {
  heap_t *heap = malloc(sizeof(heap_t));
  if (heap == NULL) {
    return NULL;
  }

  heap->slab_size = slab_size;
  heap->slab_num = slab_num;
  heap->total_size = slab_size * slab_num;

  heap->mem = malloc(heap->total_size);
  if (heap->mem == NULL) {
    return NULL;
  }

  heap->slabs = malloc(heap->slab_num * sizeof(slab_t));
  if (heap->slabs == NULL) {
    return NULL;
  }

  for (int slab_i = 0; slab_i < heap->slab_num; slab_i++) {
    heap->slabs[slab_i].chunk_size = 0;
    heap->slabs[slab_i].mem = heap->mem + (slab_i * heap->slab_size);
    heap->slabs[slab_i].open_field = heap->slabs[slab_i].mem;
    heap->slabs[slab_i].free_chunks = NULL;
  }

  return heap;
}

size_t
calc_min_chunk_size(size_t desired_size) {
  size_t min_chunk_size = 1;
  while (min_chunk_size < desired_size) {
    min_chunk_size *= 2;
  }
  return min_chunk_size;
}

bool
is_slab_full(heap_t *heap, uint32_t slab_i) {
  slab_t *slab = &(heap->slabs[slab_i]);
  if (slab->open_field - slab->mem >= heap->slab_size &&
      slab->free_chunks == NULL) {
    return true;
  }

  return false;
}

void*
alloc_chunk_from_slab(heap_t *heap, uint32_t slab_i) {
  slab_t *slab = &(heap->slabs[slab_i]);
  void *chunk;
  if (slab->free_chunks != NULL) {
    chunk = slab->free_chunks;
    slab->free_chunks = *(void **)chunk;
    return chunk;
  }

  if (slab->open_field - slab->mem >= heap->slab_size) {
    return NULL;
  }

  chunk = slab->open_field;
  slab->open_field += slab->chunk_size;

  return chunk;
}

void*
alloc_chunk(heap_t *heap, size_t size) {
  size = calc_min_chunk_size(size);
  int best_slab = -1;

  for (int slab_i = 0; slab_i < heap->slab_num; slab_i++) {
    if (heap->slabs[slab_i].chunk_size > size &&
        !is_slab_full(heap, slab_i) &&
        (best_slab < 0 ||
         heap->slabs[slab_i].chunk_size < heap->slabs[best_slab].chunk_size)) {
      best_slab = slab_i;
    }

    if (heap->slabs[slab_i].chunk_size == 0) {
      heap->slabs[slab_i].chunk_size = size;
    }

    if (heap->slabs[slab_i].chunk_size == size &&
        !is_slab_full(heap, slab_i)) {
      return alloc_chunk_from_slab(heap, slab_i);
    }
  }

  if (best_slab != -1) {
    return alloc_chunk_from_slab(heap, best_slab);
  }

  return NULL;
}

void
free_chunk(heap_t *heap, void *chunk) {
  int slab_i = (chunk - heap->mem) / heap->slab_size;
  slab_t *slab = &(heap->slabs[slab_i]);
  void **chunk_as_ptr = chunk;

  *chunk_as_ptr = slab->free_chunks;
  slab->free_chunks = chunk_as_ptr;
}
