#include "arenas.h"

#include <stdbool.h>
#include <stdio.h>

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

/*
 * This function calculates the maximum chunk size and size_class that can fit
 * in a "size" of memory.
 */
void
calc_max_size_class_and_max_chunk_size(size_t size, uint8_t *size_class,
    size_t *chunk_size) {
  *size_class = 0;
  *chunk_size = 1;

  while ((*chunk_size) <= size) {
    (*size_class) += 1;
    (*chunk_size) *= 2;
  }

  (*size_class) -= 1;
  (*chunk_size) /= 2;
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
  uint8_t max_size_class;
  size_t max_chunk_size;
  uint8_t *mem;

  for (int arena_i = 0; arena_i < heap->arena_num; arena_i++) {
    heap->arenas[arena_i].arena_size = heap->arena_size;
    pthread_mutex_init(&(heap->arenas[arena_i].mutex), NULL);

    // Arenas cannot contain a chunk with a class size larger than the class
    // size which fits the whole arena.
    calc_max_size_class_and_max_chunk_size(heap->arena_size, &max_size_class,
        &max_chunk_size);
    heap->arenas[arena_i].max_size_class = max_size_class;

    // We need a table of linked lists with an entry for each class size.
    heap->arenas[arena_i].free_chunks = malloc((max_size_class + 1) * sizeof(void *));
    if (heap->arenas[arena_i].free_chunks == NULL) {
      return NULL;
    }
    for (int free_i = 0; free_i < max_size_class; free_i++) {
      heap->arenas[arena_i].free_chunks[free_i] = NULL;
    }

    // The memory that is allocated for that arena is placed in the free list.
    mem = heap->mem + (arena_i * heap->arena_size);

    size_t remaining_mem = heap->arena_size;

    // We break up the available memory into chunks and insert those chunks in
    // the free list. We make the chunks as large as possible. (very greedy
    // algorithm here)
    while (remaining_mem > MIN_CHUNK_SIZE) {
      calc_max_size_class_and_max_chunk_size(remaining_mem, &max_size_class,
          &max_chunk_size);
      *mem = max_size_class;
      heap->arenas[arena_i].free_chunks[max_size_class] = mem + 1;
      *(uint8_t **)(mem + 1) = NULL;
      mem += max_chunk_size;
      remaining_mem -= max_chunk_size;
    }
  }

  return heap;
}

void
destroy_heap(heap_t *heap) {
  // First walk over the arenas and all they contain.
  if (heap->arenas != NULL) {
    for (int arena_i = 0; arena_i < heap->arena_num; arena_i++) {
      if (heap->arenas[arena_i].free_chunks != NULL) {
        free(heap->arenas[arena_i].free_chunks);
        heap->arenas[arena_i].free_chunks = NULL;
      }
    }
    free(heap->arenas);
    heap->arenas = NULL;
  }

  if (heap->mem != NULL) {
    free(heap->mem);
    heap->mem = NULL;
  }

  free(heap);
}

void *
alloc_chunk_from_arena(arena_t *arena, size_t chunk_size,
    uint8_t chunk_size_class) {
  uint8_t *chunk = NULL;
  uint8_t size_class = chunk_size_class;
  size_t size = chunk_size;

  // First, we look for the smallest available free chunk which would satisfy
  // the request.
  while (arena->free_chunks[size_class] == NULL) {
    if (size_class >= arena->max_size_class) {
      return NULL;
    }
    size_class++;
    size *= 2;
  }

  // Then we break the chunk in half until it is the smallest chunk that can
  // satisfy the request.
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

  // First, we initialize a list of all the arenas we can try to allocate from
  int tried_arenas_num = 0;
  bool *tried_arenas = malloc(sizeof(bool) * heap->arena_num);
  for (int i = 0; i < heap->arena_num; i++) {
    tried_arenas[i] = false;
  }

  // Then we make sure to try every arena.
  int arena_i = 0;
  while (tried_arenas_num < heap->arena_num) {
    // If we have tried this arena already, don't try it again.
    if (tried_arenas[arena_i]) {
      continue;
    }

    // If we can get a lock on this arena, try to allocate from it.
    if (pthread_mutex_trylock(&(heap->arenas[arena_i].mutex)) == 0) {
      // Try to allocate from this arena.
      chunk = alloc_chunk_from_arena(&(heap->arenas[arena_i]), chunk_size,
                                     chunk_size_class);
      pthread_mutex_unlock(&(heap->arenas[arena_i].mutex));
      // Mark this arena as tried.
      tried_arenas[arena_i] = true;
      tried_arenas_num++;

      if (chunk != NULL) {
        free(tried_arenas);
        return chunk;
      }
    }

    arena_i++;
    if (arena_i >= heap->arena_num) {
      arena_i = 0;
    }
  }

  free(tried_arenas);
  return NULL;
}

void
free_chunk_from_arena(arena_t *arena, void *chunk) {
  uint8_t size_class = *((uint8_t *)chunk - 1);
  void **chunk_as_ptr = chunk;
  // Put that chunk on the free list.
  *chunk_as_ptr = arena->free_chunks[size_class];
  arena->free_chunks[size_class] = (uint8_t *)chunk_as_ptr;
}

void
free_chunk(heap_t *heap, void *chunk) {
  // First, calculate in what arena this chunk belongs.
  int arena_i = ((uint8_t *)chunk - heap->mem) / heap->arena_size;

  pthread_mutex_lock(&(heap->arenas[arena_i].mutex));
  free_chunk_from_arena(&(heap->arenas[arena_i]), chunk);
  pthread_mutex_unlock(&(heap->arenas[arena_i].mutex));
}

void*
free_and_alloc_chunk(heap_t *heap, void *chunk, size_t size) {
  // First, calculate in what arena this chunk belongs.
  int arena_i = ((uint8_t *)chunk - heap->mem) / heap->arena_size;
  // Get a lock on that arena
  pthread_mutex_lock(&(heap->arenas[arena_i].mutex));
  free_chunk_from_arena(&(heap->arenas[arena_i]), chunk);

  size_t chunk_size;
  uint8_t chunk_size_class;
  calc_size_class_and_min_chunk_size(size, &chunk_size_class, &chunk_size);
  chunk = alloc_chunk_from_arena(&(heap->arenas[arena_i]), chunk_size,
                                 chunk_size_class);
  // Unlock the arena
  pthread_mutex_unlock(&(heap->arenas[arena_i].mutex));
  return chunk;
}
