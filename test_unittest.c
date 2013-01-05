#include "unittest.h"

void
test_assert_true() {
  assert_true(0 == 0);
}

void
test_assert_false() {
  assert_false(0 != 0);
}

void
test_assert_equals() {
  assert_equals(0, 0);
}

void
test_assert_not_equals() {
  assert_not_equals(0, 1);
}

void
test_assert_is() {
  int a = 4;
  assert_is(a, a);
}

void
test_assert_not_is() {
  int a = 4;
  int b = 4;
  assert_not_is(a, b);
}

void
test_unittest_abs() {
  assert_equals(UNITTEST_ABS(-4.0), 4.0);
  assert_equals(UNITTEST_ABS(4.0), 4.0);
}

void
test_assert_float_equals() {
  assert_float_equals(4.0001, 4.0, 0.001);
}

void
test_assert_float_not_equals() {
  assert_float_not_equals(4.0001, 5.0, 0.001);
}

void
test_assert_float_equals_p() {
  assert_float_equals_p(4.0001, 4.0);
}

void
test_assert_float_not_equals_p() {
  assert_float_not_equals_p(4.0001, 5.0);
}

void
test_assert_is_null() {
  assert_is_null(NULL);
}

void
test_assert_is_not_null() {
  assert_is_not_null(NULL+1);
}

void
test_assert_equals_buf() {
  char str1[] = "abcd";
  char str2[] = "abcd";

  assert_equals_buf(str1, str2, 4);
}

void
test_assert_not_equals_buf() {
  char str1[] = "abcd";
  char str2[] = "abce";

  assert_not_equals_buf(str1, str2, 4);
}

int
main(int argc, char **argv) {
  test_assert_true();
  test_assert_false();
  test_assert_equals();
  test_assert_not_equals();
  test_assert_is();
  test_assert_not_is();
  test_unittest_abs();
  test_assert_float_equals();
  test_assert_float_not_equals();
  test_assert_float_equals_p();
  test_assert_float_not_equals_p();
  test_assert_is_null();
  test_assert_is_not_null();
  test_assert_equals_buf();
  test_assert_not_equals_buf();
  return 0;
}
