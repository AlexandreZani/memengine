#include "unittest.h"
#include "hash.h"

void
test_hash_32_equal() {
  uint32_t data[] = {0x01234, 0x5678, 0x9abc, 0xdef0};
  assert_equals(hash_32(data, 4), hash_32(data, 4));

  assert_equals(hash_32(data, 0), hash_32(data, 0));
}

void
test_hash_32_not_equal() {
  uint32_t data1[] = {0x01234, 0x5678, 0x9abc, 0xdef1};
  uint32_t data2[] = {0x01234, 0x5678, 0x9abc, 0xdef0};
  assert_not_equals(hash_32(data1, 4), hash_32(data2, 4));

  uint32_t data3[] = {0x01234, 0x5678, 0x9abc, 0xdef1};
  uint32_t data4[] = {0x01235, 0x5678, 0x9abc, 0xdef0};
  assert_not_equals(hash_32(data3, 4), hash_32(data4, 4));

  assert_not_equals(hash_32(data3, 3), hash_32(data3, 4));
}

int
main(int argc, char **argv) {
  test_hash_32_equal();
  test_hash_32_not_equal();
  return 0;
}
