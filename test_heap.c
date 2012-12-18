#include "heap.h"

#include "unittest.h"

void
test_create_heap() {
  heap_t *heap = create_heap(1024 * 1024, 500);
  assert_equals(500 * 1024 * 1024, heap->total_size);
  assert_equals(1024 * 1024, heap->slab_size);
  assert_equals(500, heap->slab_num);
  assert_is_not_null(heap->mem);
}

int
main(int argc, char **argv) {
  test_create_heap();

  return 0;
}
