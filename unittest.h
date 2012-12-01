#ifndef __UNITTEST_H
#define __UNITTEST_H

#include <assert.h>

#define UNITTEST_ABS(a) ((a) >= 0 ? (a) : -1 * (a))

#define assert_equals(a, b) assert((a) == (b))
#define assert_not_equals(a, b) assert((a) != (b))

#define assert_is(a, b) assert(&(a) == &(b))
#define assert_not_is(a, b) assert(&(a) != &(b))

#define assert_float_equals(a, b, p) assert(UNITTEST_ABS((a) - (b)) <= (p))
#define assert_float_not_equals(a, b, p) assert(UNITTEST_ABS((a) - (b)) > (p))

#define UNITTEST_FLOAT_PRECISION 0.0001

#define assert_float_equals_p(a, b) assert_float_equals(a, b, UNITTEST_FLOAT_PRECISION)
#define assert_float_not_equals_p(a, b) assert_float_not_equals(a, b, UNITTEST_FLOAT_PRECISION)

#endif // __UNITTEST_H
