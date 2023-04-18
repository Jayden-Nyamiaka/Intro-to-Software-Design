#include "test_util.h"
#include "list.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>


void test_list_size_vector() {
  list_t *lst = list_init(4, free);
  assert(list_size(lst) == 0);
  // Add
  vector_t *v = malloc(sizeof(vector_t));
  *v = (vector_t) {5, 7.7};
  list_add(lst, s);
  assert(list_size(lst) == 1);
  // Remove
  vector_t *out = (vector_t *) list_remove(lst, 0);
  assert(list_size(lst) == 0);
  assert(out->x == 5 && out->y == 7.7);
  free(out);
  // Add again
  v = malloc(sizeof(vector_t));
  *v = VEC_ZERO;
  list_add(lst, v);
  assert(list_size(lst) == 1);
  assert(v->y == 0);
  v = malloc(sizeof(vector_t));
  *v = (vector_t) {1.2, 2.1};
  list_add(lst, v);
  // Modify
  assert(list_size(lst) == 2);
  list_free(lst);
}

void add_NULL(void *lst) { list_add((list *)lst, NULL); }
void init_capacity_zero(void *lst) { list_init(0); }
void remove_out_of_bounds(void *lst) {
  list_remove((list *)lst, list_size((list *)lst));
}
void remove_empty(void *lst) { list_remove((list *)lst, 0); }

void test_list_add_remove() {
  list *lst = list_init(2);
  // Capacity 0
  test_assert_fail(init_capacity_zero, lst);
  // add NULL
  test_assert_fail(add_NULL, lst);
  // Add
  star *s = make_star(5, 10, 15, 20, (vector_t){25, 30});
  list_add(lst, s);
  s = make_star(3, 10, 15, 20, (vector_t){25, 30});
  list_add(lst, s);
  // Should resize
  s = make_star(7, 10, 15, 20, (vector_t){25, 30});
  list_add(lst, s);
  // Remove from end
  s = list_remove(lst, 2);
  free_star(s);
  // Remove from front
  s = list_remove(lst, 0);
  free_star(s);
  // Remove index > size
  test_assert_fail(remove_out_of_bounds, lst);
  // Remove last star
  assert(list_size(lst) == 1);
  s = list_remove(lst, 0);
  free_star(s);
  // Remove from empty
  test_assert_fail(remove_empty, lst);
  list_free(lst);
}

void get_out_of_bounds(void *lst) {
  list_get((list *)lst, list_size((list *)lst));
}

void test_list_get_add_large() {
  // Add LARGE stars
  const size_t LARGE = 1000;
  list *lst = list_init(10);
  for (size_t i = 0; i < LARGE; i++) {
    list_add(lst, make_star(i + 2, 10 + i, 15 + i, 0,
                                 (vector_t){(25 + i) / 5.0, (30 + i) / 5.0}));
  }
  // Get index out of bounds
  test_assert_fail(get_out_of_bounds, lst);
  // Get first and last
  star *s = list_get(lst, 0);
  assert(s->sides == 2);
  s = list_get(lst, LARGE - 1);
  assert(s->sides == LARGE + 1);
  list_free(lst);
}

int main(int argc, char *argv[]) {
  // Run all tests? True if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_make_star_update)
  DO_TEST(test_list_size)
  DO_TEST(test_list_add_remove)
  DO_TEST(test_list_get_add_large)

  puts("tools_test PASS");
}
