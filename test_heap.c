#include "heap.h"

#include "unittest.h"

void
test_create_heap() {
  heap_t *heap = create_heap(128, 3);
  assert_is_not_null(heap);
  assert_equals(128 * 3, heap->total_size);
  assert_equals(128, heap->slab_size);
  assert_equals(3, heap->slab_num);
  assert_is_not_null(heap->mem);
  assert_is_not_null(heap->slabs);

  for (int i = 0; i < heap->slab_num; i++) {
    assert_equals(heap->slabs[i].chunk_size, 0);
    assert_is_not_null(heap->slabs[i].mem);
    assert_equals(heap->slabs[i].mem, heap->slabs[i].open_field);
    assert_is_null(heap->slabs[i].free_chunks);
  }
  // This for some reason catches insufficient memory allocated for slabs.
  // Don't ask me why.
  malloc(1);
}

void
test_alloc_chunk_basic() {
  heap_t *heap = create_heap(128, 1);
  assert_is_not_null(alloc_chunk(heap, 10));
}

void
test_alloc_chunk_fill_slabs() {
  heap_t *heap = create_heap(128, 2);

  // First allocate every chunk.
  for (int i = 0; i < 2 * 128 / 16; i++) {
    assert_is_not_null(alloc_chunk(heap, 10));
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 10));
}

void
test_alloc_chunk_fill_one_size() {
  heap_t *heap = create_heap(128, 2);
  
  // Allocate one slab for 8 bytes chunks
  assert_is_not_null(alloc_chunk(heap, 8));

  // Fill up a slab with 16 bytes chunks
  for (int i = 0; i < 128 / 16; i++) {
    assert_is_not_null(alloc_chunk(heap, 10));
  }

  // Now, there should not be any space left for 16 bytes chunks
  assert_is_null(alloc_chunk(heap, 10));
  
  // But plenty left for 8-byte chunks.
  assert_is_not_null(alloc_chunk(heap, 8));
}

void
test_alloc_chunk_waste_space_if_necessary() {
  heap_t *heap = create_heap(128, 2);

  // Allocate one slab for 16 bytes chunks
  assert_is_not_null(alloc_chunk(heap, 16));

  // Fill up a slab with 8 bytes chunks
  for (int i = 0; i < 128 / 8; i++) {
    assert_is_not_null(alloc_chunk(heap, 8));
  }

  // Now we should be allocating from the 16 bytes slab
  assert_is_not_null(alloc_chunk(heap, 8));
}

void
test_free_chunk() {
  heap_t *heap = create_heap(128, 2);
  void *chunk0, *chunk3, *chunk10, *chunk15;

  // First allocate every chunk.
  for (int i = 0; i < 2 * 128 / 16; i++) {
    if (i == 0) {
      chunk0 = alloc_chunk(heap, 10);
      assert_is_not_null(chunk0);
    } else if (i == 3) {
      chunk3 = alloc_chunk(heap, 10);
      assert_is_not_null(chunk3);
    } else if (i == 10) {
      chunk10 = alloc_chunk(heap, 10);
      assert_is_not_null(chunk10);
    } else if (i == 15) {
      chunk15 = alloc_chunk(heap, 10);
      assert_is_not_null(chunk15);
    } else {
      assert_is_not_null(alloc_chunk(heap, 10));
    }
  }
  // Now there should not be any memory left.
  assert_is_null(alloc_chunk(heap, 10));

  free_chunk(heap, chunk0);
  free_chunk(heap, chunk3);
  free_chunk(heap, chunk10);
  free_chunk(heap, chunk15);

  assert_is_not_null(alloc_chunk(heap, 10));
  assert_is_not_null(alloc_chunk(heap, 10));
  assert_is_not_null(alloc_chunk(heap, 10));
  assert_is_not_null(alloc_chunk(heap, 10));
  assert_is_null(alloc_chunk(heap, 10));
}

int
main(int argc, char **argv) {
  test_create_heap();
  test_alloc_chunk_basic();
  test_alloc_chunk_fill_slabs();
  test_alloc_chunk_fill_one_size();
  test_alloc_chunk_waste_space_if_necessary();
  test_free_chunk();

  return 0;
}
