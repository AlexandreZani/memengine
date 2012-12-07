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

int
main(int argc, char **argv) {
  test_hash_32_equal();
  test_hash_32_not_equal();
  test_hash_8_equal();
  test_hash_8_equal_odd_sizes();
  return 0;
}