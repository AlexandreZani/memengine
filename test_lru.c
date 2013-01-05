#include "lru.h"

#include "unittest.h"
#include <stdarg.h>

void
test_create_lru_queue() {
  lru_queue_t *lru_queue = create_lru_queue();
  assert_is_not_null(lru_queue);
  assert_is_null(lru_queue->most_recent);
  assert_is_null(lru_queue->least_recent);
}

/*
 * A utility function to make it easier to test the order of the lru queue.
 * It must take at least 2 parameters.
 */
void
assert_queue_equals(lru_queue_t *lru_queue, int num, ...) {
  cache_entry_t **expected = malloc(sizeof(cache_entry_t *) * num);
  va_list args;
  va_start(args, num);
  for (int i = 0; i < num; i++) {
    expected[i] = va_arg(args, cache_entry_t *);
  }
  va_end(args);

  cache_entry_t *q_pos = lru_queue->most_recent;
  for (int i = 0; i < num; i++) {
    assert_equals(expected[i], q_pos);
    if (i == 0) {
      assert_is_null(q_pos->lru_more_recent);
      assert_equals(expected[i+1], q_pos->lru_less_recent);
    } else if (i == num - 1) {
      assert_is_null(q_pos->lru_less_recent);
      assert_equals(expected[i-1], q_pos->lru_more_recent);
    } else {
      assert_equals(expected[i-1], q_pos->lru_more_recent);
      assert_equals(expected[i+1], q_pos->lru_less_recent);
    }
    assert_equals(expected[0], lru_queue->most_recent);
    assert_equals(expected[num-1], lru_queue->least_recent);
    q_pos = q_pos->lru_less_recent;
  }
  free(expected);
}

void
test_add_to_queue() {
  lru_queue_t *lru_queue = create_lru_queue();

  cache_entry_t *cache_entry_1 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(cache_entry_1, " ", 1, " ", 1);

  lru_add_new_entry(lru_queue, cache_entry_1);

  assert_equals(cache_entry_1, lru_queue->most_recent);
  assert_equals(cache_entry_1, lru_queue->least_recent);

  cache_entry_t *cache_entry_2 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(cache_entry_2, " ", 1, " ", 1);
  
  lru_add_new_entry(lru_queue, cache_entry_2);

  assert_equals(cache_entry_2, lru_queue->most_recent);
  assert_equals(cache_entry_1, lru_queue->least_recent);

  assert_equals(cache_entry_2->lru_less_recent, cache_entry_1);
  assert_equals(cache_entry_1->lru_more_recent, cache_entry_2);

  assert_queue_equals(lru_queue, 2, cache_entry_2, cache_entry_1);
}

void
test_mark_as_used() {
  lru_queue_t *lru_queue = create_lru_queue();

  cache_entry_t *c1 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(c1, " ", 1, " ", 1);
  cache_entry_t *c2 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(c2, " ", 1, " ", 1);
  cache_entry_t *c3 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(c3, " ", 1, " ", 1);
  
  lru_add_new_entry(lru_queue, c1);
  lru_add_new_entry(lru_queue, c2);
  lru_add_new_entry(lru_queue, c3);
  assert_queue_equals(lru_queue, 3, c3, c2, c1);

  assert_true(lru_bump_used_entry(lru_queue, c2));
  assert_queue_equals(lru_queue, 3, c2, c3, c1);

  assert_true(lru_bump_used_entry(lru_queue, c1));
  assert_queue_equals(lru_queue, 3, c1, c2, c3);

  assert_true(lru_bump_used_entry(lru_queue, c1));
  assert_queue_equals(lru_queue, 3, c1, c2, c3);
}

void
test_very_short_lru_queue() {
  lru_queue_t *lru_queue = create_lru_queue();

  cache_entry_t *c1 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(c1, " ", 1, " ", 1);
  cache_entry_t *c2 = malloc(calc_cache_entry_size(1, 1));
  assemble_cache_entry(c2, " ", 1, " ", 1);

  lru_add_new_entry(lru_queue, c1);
  assert_true(lru_bump_used_entry(lru_queue, c1));
  assert_equals(lru_queue->most_recent, c1);
  assert_equals(lru_queue->least_recent, c1);
  assert_is_null(c1->lru_more_recent);
  assert_is_null(c1->lru_less_recent);

  lru_add_new_entry(lru_queue, c2);
  assert_queue_equals(lru_queue, 2, c2, c1);

  assert_true(lru_bump_used_entry(lru_queue, c1));
  assert_queue_equals(lru_queue, 2, c1, c2);

  assert_true(lru_bump_used_entry(lru_queue, c1));
  assert_queue_equals(lru_queue, 2, c1, c2);
}

int
main(int argc, char **argv) {
  test_create_lru_queue();
  test_add_to_queue();
  test_mark_as_used();
  test_very_short_lru_queue();
  return 0;
}
