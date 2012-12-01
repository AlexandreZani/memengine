#include "unittest.h"

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

int
main(int argc, char **argv) {
  test_assert_equals();
  test_assert_not_equals();
  test_assert_is();
  test_assert_not_is();
  test_unittest_abs();
  test_assert_float_equals();
  test_assert_float_not_equals();
  test_assert_float_equals_p();
  test_assert_float_not_equals_p();
}
