#include "cache_entry.h"

#include "unittest.h"

void
test_calc_cache_entry_size() {
  assert_equals(sizeof(cache_entry_t) + 5 + 7,
                calc_cache_entry_size(5, 7));
}

void
test_assemble_cache_entry() {
  char *key = "some key";
  char *data = "some data";

  size_t cache_entry_size = calc_cache_entry_size(
      strlen(key), strlen(data));

  cache_entry_t *cache_entry = malloc(cache_entry_size);

  assemble_cache_entry(cache_entry, key, strlen(key), data, strlen(data));

  assert_equals(strlen(key), cache_entry->key_size);
  assert_equals(strlen(data), cache_entry->data_size);
  assert_equals_buf(key, cache_entry->key, cache_entry->key_size);
  assert_equals_buf(data, cache_entry->data, cache_entry->data_size);
  assert_is_null(cache_entry->hash_table_next);
  assert_is_null(cache_entry->lru_less_recent);
  assert_is_null(cache_entry->lru_more_recent);
}


int
main(int argc, char** argv) {
  test_calc_cache_entry_size();
  test_assemble_cache_entry();
  return 0;
}
