#include "cache.h"

#include "unittest.h"

void
test_cache_set_get_item() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";
  size_t key_sz = strlen(key);
  size_t data_sz = strlen(data);
  cache_set_item(cache, (uint8_t *)key, key_sz, (uint8_t *)data, data_sz);

  cache_entry_t *cache_entry = cache_get_item(cache, (uint8_t *)key, key_sz);
  assert_is_not_null(cache_entry);
}

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
