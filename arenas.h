#ifndef __ARENAS_H
#define __ARENAS_H

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

typedef struct {
  size_t arena_size;
  uint8_t *open_field;
  uint8_t *mem;
  uint8_t **free_chunks;
  uint8_t max_size_class;
  pthread_mutex_t mutex;
} arena_t;

typedef struct {
  size_t total_size;
  size_t arena_size;
  size_t arena_num;
  uint8_t *mem;
  arena_t *arenas;
} heap_t;

extern heap_t *create_heap(size_t arena_size, uint32_t arena_num);
extern void *alloc_chunk(heap_t *heap, size_t size);
extern void free_chunk(heap_t *heap, void *chunk);

#endif
