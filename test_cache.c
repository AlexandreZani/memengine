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
  assert_equals_buf(key, cache_entry->key, key_sz);
  assert_equals_buf(data, cache_entry->data, data_sz);
  destroy_cache(cache);
}

void
test_cache_set_unset_no_get_item() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";
  size_t key_sz = strlen(key);
  size_t data_sz = strlen(data);
  cache_set_item(cache, (uint8_t *)key, key_sz, (uint8_t *)data, data_sz);

  cache_entry_t *cache_entry = cache_get_item(cache, (uint8_t *)key, key_sz);
  assert_is_not_null(cache_entry);

  cache_unset_item(cache, (uint8_t *)key, key_sz);

  cache_entry = cache_get_item(cache, (uint8_t *)key, key_sz);
  assert_is_null(cache_entry);
  destroy_cache(cache);
}

void
test_cache_set_auto_free() {
  cache_t *cache = create_cache(256, 2, 128);

  uint8_t data[] = {0x01, 0x02, 0x03};
  size_t data_size = 3;

  for (uint8_t key = 0; key < 255; key++) {
    cache_set_item(cache, &key, 1, data, data_size);
    assert_is_not_null(cache_get_item(cache, &key, 1));
  }

  uint8_t key = 0;
  assert_is_null(cache_get_item(cache, &key, 1));

  destroy_cache(cache);
}

int
main(int argc, char** argv) {
  test_create_destroy_cache();
  test_cache_set_get_item();
  test_cache_set_unset_no_get_item();
  test_cache_set_auto_free();
}
