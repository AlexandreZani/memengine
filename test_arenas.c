#include "arenas.h"

#include "unittest.h"

void
test_create_heap() {
  heap_t *heap = create_heap(128, 3);
  assert_is_not_null(heap);
  assert_equals(128 * 3, heap->total_size);
  assert_equals(128, heap->arena_size);
  assert_equals(3, heap->arena_num);
  assert_is_not_null(heap->mem);
  assert_is_not_null(heap->arenas);

  for (int i = 0; i < heap->arena_num; i++) {
    assert_is_not_null(heap->arenas[i].mem);
    assert_equals(heap->arenas[i].mem, heap->arenas[i].open_field);
    assert_is_not_null(heap->arenas[i].free_chunks);
  }
  // This for some reason catches insufficient memory allocated for arenas.
  // Don't ask me why.
  malloc(1);
}

void
test_alloc_chunk_basic() {
  heap_t *heap = create_heap(128, 1);
  void *chunk1, *chunk2;
  chunk1 = alloc_chunk(heap, 15);
  chunk2 = alloc_chunk(heap, 31);
  assert_is_not_null(chunk1);
  assert_is_not_null(chunk2);
  assert_not_equals(chunk1, chunk2);

  // Checking that the header has been properly set.
  assert_equals(4, *(((uint8_t *)chunk1)-1));
  assert_equals(5, *(((uint8_t *)chunk2)-1));
}

void
test_alloc_chunk_fill_arenas() {
  heap_t *heap = create_heap(128, 2);

  // First allocate every chunk.
  for (int i = 0; i < 2 * 128 / 16; i++) {
    assert_is_not_null(alloc_chunk(heap, 15));
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 15));
}

void
test_free_chunk() {
  heap_t *heap = create_heap(128, 2);
  void *chunk0, *chunk3, *chunk10, *chunk15;

  // First allocate every chunk.
  for (int i = 0; i < 2 * 128 / 16; i++) {
    if (i == 0) {
      chunk0 = alloc_chunk(heap, 15);
      assert_is_not_null(chunk0);
    } else if (i == 3) {
      chunk3 = alloc_chunk(heap, 15);
      assert_is_not_null(chunk3);
    } else if (i == 10) {
      chunk10 = alloc_chunk(heap, 15);
      assert_is_not_null(chunk10);
    } else if (i == 15) {
      chunk15 = alloc_chunk(heap, 15);
      assert_is_not_null(chunk15);
    } else {
      assert_is_not_null(alloc_chunk(heap, 15));
    }
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 15));

  free_chunk(heap, chunk0);
  free_chunk(heap, chunk3);
  free_chunk(heap, chunk10);
  free_chunk(heap, chunk15);

  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_null(alloc_chunk(heap, 15));
}


// Implement chunk splitting.
void
test_splitting_free_chunks() {
  heap_t *heap = create_heap(256, 4);
  void *chunk0, *chunk3, *chunk10, *chunk15;

  // First allocate every chunk.
  for (int i = 0; i < 4 * 256 / 64; i++) {
    if (i == 0) {
      chunk0 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk0);
    } else if (i == 3) {
      chunk3 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk3);
    } else if (i == 10) {
      chunk10 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk10);
    } else if (i == 15) {
      chunk15 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk15);
    } else {
      assert_is_not_null(alloc_chunk(heap, 63));
    }
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 63));
  assert_is_null(alloc_chunk(heap, 31));
  assert_is_null(alloc_chunk(heap, 15));

  free_chunk(heap, chunk0);
  free_chunk(heap, chunk3);
  free_chunk(heap, chunk10);
  free_chunk(heap, chunk15);

  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));
  assert_is_not_null(alloc_chunk(heap, 15));

  assert_is_null(alloc_chunk(heap, 15));
}

void
test_splitting_free_chunks_min_size() {
  heap_t *heap = create_heap(256, 4);
  void *chunk0, *chunk3, *chunk10, *chunk15;

  // First allocate every chunk.
  for (int i = 0; i < 4 * 256 / 64; i++) {
    if (i == 0) {
      chunk0 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk0);
    } else if (i == 3) {
      chunk3 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk3);
    } else if (i == 10) {
      chunk10 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk10);
    } else if (i == 15) {
      chunk15 = alloc_chunk(heap, 63);
      assert_is_not_null(chunk15);
    } else {
      assert_is_not_null(alloc_chunk(heap, 63));
    }
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 63));
  assert_is_null(alloc_chunk(heap, 31));
  assert_is_null(alloc_chunk(heap, 15));

  free_chunk(heap, chunk0);
  free_chunk(heap, chunk3);
  free_chunk(heap, chunk10);
  free_chunk(heap, chunk15);

  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 31));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));
  assert_is_not_null(alloc_chunk(heap, 7));

  assert_is_null(alloc_chunk(heap, 15));
}
int
main(int argc, char **argv) {
  test_create_heap();
  test_alloc_chunk_basic();
  test_alloc_chunk_fill_arenas();
  test_free_chunk();
  test_splitting_free_chunks();
  test_splitting_free_chunks_min_size();
}
