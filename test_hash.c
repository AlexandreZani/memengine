#include <stdlib.h>

#include "unittest.h"
#include "hash.h"

void
test_hash_32_equal() {
  uint32_t data[] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
  assert_equals(hash_32(data, 4), hash_32(data, 4));

  assert_equals(hash_32(data, 0), hash_32(data, 0));
}

void
test_hash_32_not_equal() {
  uint32_t data1[] = { 0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210 };
  uint32_t data2[] = { 0x01234567, 0x89abcdef, 0xfedcba98, 0x76543211 };
  assert_not_equals(hash_32(data1, 4), hash_32(data2, 4));
  assert_not_equals(hash_32(data1, 4), hash_32(data1, 3));
}

void
test_hash_8_equal() {
  uint8_t data1[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
  uint8_t data2[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
  assert_equals(hash_8(data1, 8), hash_8(data2, 8));
}

void
test_hash_8_equal_odd_sizes() {
  uint8_t data[]  = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
  uint8_t data7[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0x00 };
  uint8_t data6[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0x00, 0x00 };
  uint8_t data5[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x00, 0x00 };
  uint8_t data4[] = { 0x01, 0x23, 0x45, 0x67 };
  uint8_t data3[] = { 0x01, 0x23, 0x45, 0x00 };
  uint8_t data2[] = { 0x01, 0x23, 0x00, 0x00 };
  uint8_t data1[] = { 0x01, 0x00, 0x00, 0x00 };
  uint8_t data0[] = { };

  assert_equals(hash_8(data7, 8), hash_8(data, 7));
  assert_equals(hash_8(data6, 8), hash_8(data, 6));
  assert_equals(hash_8(data5, 8), hash_8(data, 5));

  assert_equals(hash_8(data4, 4), hash_8(data, 4));
  assert_equals(hash_8(data3, 4), hash_8(data, 3));
  assert_equals(hash_8(data2, 4), hash_8(data, 2));
  assert_equals(hash_8(data1, 4), hash_8(data, 1));

  assert_equals(hash_8(data0, 0), hash_8(data, 0));
}

void
test_hash_table_create() {
  hash_table_t *table = create_hash_table(16);
  assert_equals(0xf, table->mask);
  assert_not_equals(0, table->entries);
}

void
test_hash_table_index_entry() {
  char *key = "some key";
  char *data = "some data";

  hash_table_t *table = create_hash_table(16);

  size_t entry_size = calc_cache_entry_size(strlen(key), strlen(data));
  cache_entry_t *cache_entry = malloc(entry_size);
  assemble_cache_entry(cache_entry, key, strlen(key), data, strlen(data));

  hash_table_index_entry(table, cache_entry);

  assert_equals(cache_entry,
      hash_table_get_entry(table, (uint8_t *)key, strlen(key)));
}

void
test_hash_table_get_null() {
  hash_table_t *table = create_hash_table(16);
  assert_is_null(hash_table_get_entry(table, (uint8_t *)"some key", 8));
}

void
test_hash_table_collision_resolution() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  // This test relies upon the hash algorithm operating on multiples of 2 and
  // padding with 0x00 in order to generate 2 different keys with identical
  // hashes.
  uint8_t key[] = { 0xFF, 0xAA, 0xBB, 0x00 };

  size_t entry_size_1 = calc_cache_entry_size(4, strlen(data1));
  size_t entry_size_2 = calc_cache_entry_size(3, strlen(data2));

  cache_entry_t *entry1 = malloc(entry_size_1);
  cache_entry_t *entry2 = malloc(entry_size_2);

  assemble_cache_entry(entry1, key, 4, data1, strlen(data1));
  assemble_cache_entry(entry2, key, 3, data2, strlen(data2));

  hash_table_index_entry(table, entry1);
  hash_table_index_entry(table, entry2);

  assert_equals(entry1, hash_table_get_entry(table, key, 4));
  assert_equals(entry2, hash_table_get_entry(table, key, 3));
}

/*
void
test_hash_table_collision_resolution() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  uint8_t key[] = { 0xFF, 0xAA, 0xBB, 0x00 };
  hash_table_set(table, key, 4, data1);
  hash_table_set(table, key, 3, data2);
  assert_equals(data1, hash_table_get(table, key, 4));
  assert_equals(data1, hash_table_get(table, key, 3));
}

/*
void
test_hash_table_collision_resolution() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  uint8_t key[] = { 0xFF, 0xAA, 0xBB, 0x00 };
  hash_table_set(table, key, 4, data1);
  hash_table_set(table, key, 3, data2);
  assert_equals(data1, hash_table_get(table, key, 4));
  assert_equals(data1, hash_table_get(table, key, 3));
}

void
test_hash_table_data_update() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";

  hash_table_set(table, (uint8_t *)"some key", 8, data1);
  assert_equals(data1, hash_table_get(table, (uint8_t *)"some key", 8));
  hash_table_set(table, (uint8_t *)"some key", 8, data2);
  assert_equals(data2, hash_table_get(table, (uint8_t *)"some key", 8));
}

void
test_hash_table_unset() {
  char data[] = "some data";
  hash_table_t *table = create_hash_table(16);
  hash_table_set(table, (uint8_t *)"some key", 8, data);
  assert_equals(data, hash_table_get(table, (uint8_t *)"some key", 8));
  hash_table_unset(table, (uint8_t *)"some key", 8);
  assert_equals(NULL, hash_table_get(table, (uint8_t *)"some key", 8));
}

void
test_hash_table_unset_null() {
  hash_table_t *table = create_hash_table(16);
  hash_table_unset(table, (uint8_t *)"some key", 8);
}
*/

int
main(int argc, char **argv) {
  test_hash_32_equal();
  test_hash_32_not_equal();
  test_hash_8_equal();
  test_hash_8_equal_odd_sizes();

  test_hash_table_create();
  test_hash_table_index_entry();
  test_hash_table_get_null();
  test_hash_table_collision_resolution();
  /*
  test_hash_table_data_update();
  test_hash_table_unset();
  test_hash_table_unset_null();
  */

  return 0;
}
