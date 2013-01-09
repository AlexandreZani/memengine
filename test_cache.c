#include "cache.h"

#include "unittest.h"

void
test_create_destroy_cache() {
  cache_t *cache = create_cache(1024, 10, 128);
  assert_is_not_null(cache);
  assert_is_not_null(cache->heap);
  assert_is_not_null(cache->hash_table);
  assert_is_not_null(cache->lru_queue);
  destroy_cache(cache);
}

int
main(int argc, char** argv) {
  test_create_destroy_cache();
}
