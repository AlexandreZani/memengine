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
  uint32_t data1[] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
  uint32_t data2[] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543211};
  assert_not_equals(hash_32(data1, 4), hash_32(data2, 4));
  assert_not_equals(hash_32(data1, 4), hash_32(data1, 3));
}

int
main(int argc, char **argv) {
  test_hash_32_equal();
  test_hash_32_not_equal();
  return 0;
}
