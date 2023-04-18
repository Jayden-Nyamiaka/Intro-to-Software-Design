#include "test_util.h"
#include "tools.h"
#include <assert.h>
#include <stdlib.h>

void make_invalid_star0(void *l) {
  make_star(-1, 3, 20, 0, (vector_t){200, 200});
}
void make_invalid_star1(void *l) {
  make_star(5, 3, 2, 20, (vector_t){100, 200});
}
void make_invalid_star2(void *l) {
  make_star(4, -3, 2, 360, (vector_t){900, 100});
}
void make_invalid_star3(void *l) { make_star(4, 3, 6, 30, (vector_t){-5, -5}); }

void test_make_star_update() {
  star *s = make_star(5, 10, 20, 0, (vector_t){450, 450});
  test_assert_fail(make_invalid_star0, s);
  test_assert_fail(make_invalid_star1, s);
  test_assert_fail(make_invalid_star2, s);
  test_assert_fail(make_invalid_star3, s);
  star_update(s, 0.5);
  assert(s->sides == 5);
  assert(s->velocity.x == 100);
  assert(s->velocity.y == -250);
  free_star(s);
}

void test_star_list_size() {
  star_list *lst = star_list_init(4);
  assert(star_list_size(lst) == 0);
  // Add
  star *s = make_star(5, 10, 15, 20, (vector_t){25, 30});
  star_list_add(lst, s);
  assert(star_list_size(lst) == 1);
  // Remove
  star *out = star_list_remove(lst, 0);
  assert(star_list_size(lst) == 0);
  assert(out->sides == 5);
  free_star(out);
  // Add again
  s = make_star(10, 20, 40, 0, (vector_t){100, 200});
  star_list_add(lst, s);
  assert(star_list_size(lst) == 1);
  assert(s->velocity.y == 0);
  s = make_star(7, 50, 100, 0, (vector_t){200, 300});
  star_list_add(lst, s);
  // Modify
  assert(star_list_size(lst) == 2);
  star_list_free(lst);
}

void add_NULL(void *lst) { star_list_add((star_list *)lst, NULL); }
void init_capacity_zero(void *lst) { star_list_init(0); }
void remove_out_of_bounds(void *lst) {
  star_list_remove((star_list *)lst, star_list_size((star_list *)lst));
}
void remove_empty(void *lst) { star_list_remove((star_list *)lst, 0); }

void test_star_list_add_remove() {
  star_list *lst = star_list_init(2);
  // Capacity 0
  test_assert_fail(init_capacity_zero, lst);
  // add NULL
  test_assert_fail(add_NULL, lst);
  // Add
  star *s = make_star(5, 10, 15, 20, (vector_t){25, 30});
  star_list_add(lst, s);
  s = make_star(3, 10, 15, 20, (vector_t){25, 30});
  star_list_add(lst, s);
  // Should resize
  s = make_star(7, 10, 15, 20, (vector_t){25, 30});
  star_list_add(lst, s);
  // Remove from end
  s = star_list_remove(lst, 2);
  free_star(s);
  // Remove from front
  s = star_list_remove(lst, 0);
  free_star(s);
  // Remove index > size
  test_assert_fail(remove_out_of_bounds, lst);
  // Remove last star
  assert(star_list_size(lst) == 1);
  s = star_list_remove(lst, 0);
  free_star(s);
  // Remove from empty
  test_assert_fail(remove_empty, lst);
  star_list_free(lst);
}

void get_out_of_bounds(void *lst) {
  star_list_get((star_list *)lst, star_list_size((star_list *)lst));
}

void test_star_list_get_add_large() {
  // Add LARGE stars
  const size_t LARGE = 1000;
  star_list *lst = star_list_init(10);
  for (size_t i = 0; i < LARGE; i++) {
    star_list_add(lst, make_star(i + 2, 10 + i, 15 + i, 0,
                                 (vector_t){(25 + i) / 5.0, (30 + i) / 5.0}));
  }
  // Get index out of bounds
  test_assert_fail(get_out_of_bounds, lst);
  // Get first and last
  star *s = star_list_get(lst, 0);
  assert(s->sides == 2);
  s = star_list_get(lst, LARGE - 1);
  assert(s->sides == LARGE + 1);
  star_list_free(lst);
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
  DO_TEST(test_star_list_size)
  DO_TEST(test_star_list_add_remove)
  DO_TEST(test_star_list_get_add_large)

  puts("tools_test PASS");
}
