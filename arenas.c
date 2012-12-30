#include "arenas.h"

#include <stdbool.h>

// If the chunk is too small, we can't fit a pointer in it and the free
// list breaks.
#define MIN_CHUNK_SIZE (sizeof(void *))

/*
 * Size class
 * the size class of a chunk is an 8-bit number representing how large the
 * chunk is. The size of the chunk is 2^size_class. The first byte of the
 * chunk is used to hold the size class of the chunk. The rest can be used
 * for data.
 */

void
calc_size_class_and_min_chunk_size(size_t size, uint8_t *size_class,
    size_t *chunk_size) {
  if (size < MIN_CHUNK_SIZE) {
    size = MIN_CHUNK_SIZE;
  }
  *size_class = 0;
  *chunk_size = 1;

  while ((*chunk_size - 1) < size) {
    (*size_class) += 1;
    (*chunk_size) *= 2;
  }
}

uint8_t
calc_size_class(size_t size) {
  uint8_t size_class;
  size_t chunk_size;
  calc_size_class_and_min_chunk_size(size, &size_class, &chunk_size);

  return size_class;
}

heap_t*
create_heap(size_t arena_size, uint32_t arena_num) {
  heap_t *heap = malloc(sizeof(heap_t));
  if (heap == NULL) {
    return NULL;
  }

  heap->arena_size = arena_size;
  heap->arena_num = arena_num;
  heap->total_size = arena_size * arena_num;

  heap->mem = malloc(heap->total_size);
  if (heap->mem == NULL) {
    return NULL;
  }

  heap->arenas = malloc(heap->arena_num * sizeof(arena_t));
  if (heap->arenas == NULL) {
    return NULL;
  }

  // Arenas cannot contain a chunk with a class size larger than the class
  // size which fits the whole arena.
  uint8_t max_size_class = calc_size_class(heap->arena_size);
  for (int arena_i = 0; arena_i < heap->arena_num; arena_i++) {
    heap->arenas[arena_i].max_size_class = max_size_class;
    heap->arenas[arena_i].arena_size = heap->arena_size;
    heap->arenas[arena_i].mem = heap->mem + (arena_i * heap->arena_size);
    heap->arenas[arena_i].open_field = heap->arenas[arena_i].mem;
    heap->arenas[arena_i].free_chunks = malloc((max_size_class + 1) * sizeof(void *));
    if (heap->arenas[arena_i].free_chunks == NULL) {
      return NULL;
    }
    for (int free_i = 0; free_i < max_size_class; free_i++) {
      heap->arenas[arena_i].free_chunks[free_i] = NULL;
    }
  }

  return heap;
}

void *
alloc_chunk_from_arena(arena_t *arena, size_t chunk_size,
    uint8_t chunk_size_class) {
  uint8_t *chunk = NULL;
  if (arena->free_chunks[chunk_size_class] != NULL) {
    chunk = arena->free_chunks[chunk_size_class];
    arena->free_chunks[chunk_size_class] = *(uint8_t **)chunk;
    return (void *)chunk;
  }

  if (arena->open_field - arena->mem <= (arena->arena_size - chunk_size)) {
    chunk = arena->open_field;
    *chunk = chunk_size_class;
    arena->open_field += chunk_size;
    return (void *)(chunk + 1);
  }

  uint8_t size_class = chunk_size_class;
  size_t size = chunk_size;
  while (arena->free_chunks[size_class] == NULL) {
    if (size_class >= arena->max_size_class) {
      return NULL;
    }
    size_class++;
    size *= 2;
  }

  uint8_t *big_chunk;
  void **big_chunk_as_ptr;
  while (size_class > chunk_size_class) {
    big_chunk = arena->free_chunks[size_class];
    arena->free_chunks[size_class] = *(uint8_t **)big_chunk;

    size_class--;
    size /= 2;

    *(big_chunk - 1) = size_class;
    big_chunk_as_ptr = (void *)big_chunk;
    *big_chunk_as_ptr = arena->free_chunks[size_class];
    arena->free_chunks[size_class] = (uint8_t *)big_chunk_as_ptr;

    big_chunk += size;
    *(big_chunk - 1) = size_class;
    big_chunk_as_ptr = (void *)big_chunk;
    *big_chunk_as_ptr = arena->free_chunks[size_class];
    arena->free_chunks[size_class] = (uint8_t *)big_chunk_as_ptr;
  }

  chunk = arena->free_chunks[chunk_size_class];
  arena->free_chunks[chunk_size_class] = *(uint8_t **)chunk;
  return (void *)chunk;
}

void *
alloc_chunk(heap_t *heap, size_t size) {
  void *chunk;
  size_t chunk_size;
  uint8_t chunk_size_class;
  calc_size_class_and_min_chunk_size(size, &chunk_size_class, &chunk_size);

  for (int i = 0; i < heap->arena_num; i++) {
    chunk = alloc_chunk_from_arena(&(heap->arenas[i]), chunk_size,
                                   chunk_size_class);
    if (chunk != NULL) {
      return chunk;
    }
  }

  return NULL;
}

void
free_chunk(heap_t *heap, void *chunk) {
  int arena_i = ((uint8_t *)chunk - heap->mem) / heap->arena_size;
  arena_t *arena = &(heap->arenas[arena_i]);

  uint8_t size_class = *((uint8_t *)chunk - 1);
  void **chunk_as_ptr = chunk;
  *chunk_as_ptr = arena->free_chunks[size_class];
  arena->free_chunks[size_class] = (uint8_t *)chunk_as_ptr;
}
