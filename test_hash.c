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
test_hash_table_collision_resolution_null() {
  hash_table_t *table = create_hash_table(16);

  char data[] = "some data";
  // This test relies upon the hash algorithm operating on multiples of 2 and
  // padding with 0x00 in order to generate 2 different keys with identical
  // hashes.
  uint8_t key[] = { 0xFF, 0xAA, 0xBB, 0x00 };

  size_t entry_size = calc_cache_entry_size(4, strlen(data));
  cache_entry_t *entry = malloc(entry_size);
  assemble_cache_entry(entry, key, 4, data, strlen(data));
  hash_table_index_entry(table, entry);

  assert_equals(entry, hash_table_get_entry(table, key, 4));
  assert_is_null(hash_table_get_entry(table, key, 3));
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

void
test_hash_table_data_update_collisions() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  char data3[] = "some more data";
  // This test relies upon the hash algorithm operating on multiples of 2 and
  // padding with 0x00 in order to generate 2 different keys with identical
  // hashes.
  uint8_t key[] = { 0xFF, 0xAA, 0x00, 0x00 };

  size_t key_size1 = 4;
  size_t key_size2 = 3;
  size_t key_size3 = 2;

  size_t entry_size_1 = calc_cache_entry_size(key_size1, strlen(data1));
  size_t entry_size_2 = calc_cache_entry_size(key_size2, strlen(data2));
  size_t entry_size_3 = calc_cache_entry_size(key_size3, strlen(data3));

  cache_entry_t *entry1 = malloc(entry_size_1);
  cache_entry_t *entry2 = malloc(entry_size_2);
  cache_entry_t *entry3 = malloc(entry_size_3);

  assemble_cache_entry(entry1, key, key_size1, (uint8_t *)data1, strlen(data1));
  assemble_cache_entry(entry2, key, key_size2, (uint8_t *)data2, strlen(data2));
  assemble_cache_entry(entry3, key, key_size3, (uint8_t *)data3, strlen(data3));

  assert_is_null(hash_table_index_entry(table, entry1));
  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_is_null(hash_table_index_entry(table, entry2));
  assert_equals(entry2, hash_table_get_entry(table, key, key_size2));
  assert_is_null(hash_table_index_entry(table, entry3));

  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_equals(entry2, hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  char new_data1[] = "some new_data";
  char new_data2[] = "some other new_data";
  char new_data3[] = "some more new_data";

  size_t new_entry_size_1 = calc_cache_entry_size(key_size1, strlen(new_data1));
  size_t new_entry_size_2 = calc_cache_entry_size(key_size2, strlen(new_data2));
  size_t new_entry_size_3 = calc_cache_entry_size(key_size3, strlen(new_data3));

  cache_entry_t *new_entry1 = malloc(new_entry_size_1);
  cache_entry_t *new_entry2 = malloc(new_entry_size_2);
  cache_entry_t *new_entry3 = malloc(new_entry_size_3);

  assemble_cache_entry(new_entry1, key, key_size1, (uint8_t *)new_data1, strlen(data1));
  assemble_cache_entry(new_entry2, key, key_size2, (uint8_t *)new_data2, strlen(data2));
  assemble_cache_entry(new_entry3, key, key_size3, (uint8_t *)new_data3, strlen(data3));

  assert_equals(entry2, hash_table_index_entry(table, new_entry2));
  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_equals(new_entry2, hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  assert_equals(entry1, hash_table_index_entry(table, new_entry1));
  assert_equals(new_entry1, hash_table_get_entry(table, key, key_size1));
  assert_equals(new_entry2, hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  assert_equals(entry3, hash_table_index_entry(table, new_entry3));
  assert_equals(new_entry1, hash_table_get_entry(table, key, key_size1));
  assert_equals(new_entry2, hash_table_get_entry(table, key, key_size2));
  assert_equals(new_entry3, hash_table_get_entry(table, key, key_size3));
}

void
test_hash_table_data_update() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  char key[] = "some_key";

  size_t entry_size_1 = calc_cache_entry_size(strlen(key), strlen(data1));
  size_t entry_size_2 = calc_cache_entry_size(strlen(key), strlen(data2));

  cache_entry_t *entry1 = malloc(entry_size_1);
  cache_entry_t *entry2 = malloc(entry_size_2);

  assemble_cache_entry(entry1, (uint8_t *)key, strlen(key), (uint8_t *)data1,
      strlen(data1));
  assemble_cache_entry(entry2, (uint8_t *)key, strlen(key), (uint8_t *)data2,
      strlen(data2));

  hash_table_index_entry(table, entry1);
  // Check that the first entry was correctly indexed.
  assert_equals(entry1, hash_table_get_entry(table, (uint8_t *)key,
        strlen(key)));

  // Check that the first entry is returned when the second entry is indexed
  // So that the first entry may be freed by the caller.
  assert_equals(entry1, hash_table_index_entry(table, entry2));

  // Check that second entry was correctly indexed.
  assert_equals(entry2, hash_table_get_entry(table, (uint8_t *)key, strlen(key)));
}

void
test_hash_table_deindex() {
  hash_table_t *table = create_hash_table(16);
  char data[] = "some data";
  char key[] = "some key";

  size_t entry_size = calc_cache_entry_size(strlen(key), strlen(data));
  cache_entry_t *entry = malloc(entry_size);

  assemble_cache_entry(entry, (uint8_t *)key, strlen(key), (uint8_t *)data,
      strlen(data));
  hash_table_index_entry(table, entry);

  assert_equals(entry, hash_table_get_entry(table, (uint8_t *)key, strlen(key)));

  assert_equals(entry, hash_table_deindex(table, (uint8_t *)key, strlen(key)));

  assert_is_null(hash_table_get_entry(table, (uint8_t *)key, strlen(key)));
}

void
test_hash_table_deindex_null() {
  hash_table_t *table = create_hash_table(16);
  char key[] = "some key";
  assert_is_null(hash_table_deindex(table, (uint8_t *)key, strlen(key)));
}

void
test_hash_table_deindex_collisions() {
  hash_table_t *table = create_hash_table(16);

  char data1[] = "some data";
  char data2[] = "some other data";
  char data3[] = "some more data";
  // This test relies upon the hash algorithm operating on multiples of 2 and
  // padding with 0x00 in order to generate 2 different keys with identical
  // hashes.
  uint8_t key[] = { 0xFF, 0xAA, 0x00, 0x00 };

  size_t key_size1 = 4;
  size_t key_size2 = 3;
  size_t key_size3 = 2;

  size_t entry_size_1 = calc_cache_entry_size(key_size1, strlen(data1));
  size_t entry_size_2 = calc_cache_entry_size(key_size2, strlen(data2));
  size_t entry_size_3 = calc_cache_entry_size(key_size3, strlen(data3));

  cache_entry_t *entry1 = malloc(entry_size_1);
  cache_entry_t *entry2 = malloc(entry_size_2);
  cache_entry_t *entry3 = malloc(entry_size_3);

  assemble_cache_entry(entry1, key, key_size1, (uint8_t *)data1, strlen(data1));
  assemble_cache_entry(entry2, key, key_size2, (uint8_t *)data2, strlen(data2));
  assemble_cache_entry(entry3, key, key_size3, (uint8_t *)data3, strlen(data3));

  assert_is_null(hash_table_index_entry(table, entry1));
  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_is_null(hash_table_index_entry(table, entry2));
  assert_equals(entry2, hash_table_get_entry(table, key, key_size2));
  assert_is_null(hash_table_index_entry(table, entry3));

  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_equals(entry2, hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  assert_equals(entry2, hash_table_deindex(table, key, key_size2));
  assert_equals(entry1, hash_table_get_entry(table, key, key_size1));
  assert_is_null(hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  assert_equals(entry1, hash_table_deindex(table, key, key_size1));
  assert_is_null(hash_table_get_entry(table, key, key_size1));
  assert_is_null(hash_table_get_entry(table, key, key_size2));
  assert_equals(entry3, hash_table_get_entry(table, key, key_size3));

  assert_equals(entry3, hash_table_deindex(table, key, key_size3));
  assert_is_null(hash_table_get_entry(table, key, key_size1));
  assert_is_null(hash_table_get_entry(table, key, key_size2));
  assert_is_null(hash_table_get_entry(table, key, key_size3));
}

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
  test_hash_table_collision_resolution_null();
  test_hash_table_data_update_collisions();
  test_hash_table_data_update();
  test_hash_table_deindex();
  test_hash_table_deindex_collisions();
  test_hash_table_deindex_null();
  /*
  test_hash_table_unset_null();
  */

  return 0;
}
