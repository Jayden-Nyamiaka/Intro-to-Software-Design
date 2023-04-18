#include "sort.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t NUM_TESTS =
    100; // The number of experiments per test to run (with random arrays)

#define DO_RANDOM_TEST(TYPE, FUNC, SIZE, ...)                                  \
  for (size_t i = 0; i < NUM_TESTS; i++) {                                     \
    TYPE *arr = get_random_##TYPE##_array(SIZE);                               \
    test_##FUNC((void *)arr, SIZE, __VA_ARGS__);                               \
    TYPE##_arr_free(arr, SIZE);                                                \
  }

const int INT_TEST_ARRAY[] = {15, 10, -20, 15,  5,  15, 2,
                              15, 15, 15,  200, 15, 150};
const int INT_TEST_ARRAY_SORTED[] = {-20, 2,  5,  10, 15,  15, 15,
                                     15,  15, 15, 15, 150, 200};
const size_t INT_TEST_ARRAY_SIZE = 13;

const char *STR_TEST_ARRAY[] = {"aaaa", "aa", "h", "aaaa", "a", "aaaa", "l",
                                "d",    "d",  "d", "p",    "d", "m"};
const char *STR_TEST_ARRAY_SORTED[] = {
    "a", "aa", "aaaa", "aaaa", "aaaa", "d", "d", "d", "d", "h", "l", "m", "p"};
const size_t STR_TEST_ARRAY_SIZE = 13;

void print_int_array(size_t length, int arr[]) {
  for (int i = 0; i < length; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

void print_str_array(size_t length, char *arr[]) {
  for (int i = 0; i < length; i++) {
    printf("%s ", arr[i]);
  }
  printf("\n");
}

// Functions for duplicating the above test cases
int *get_int_array() {
  int *test_array = malloc(INT_TEST_ARRAY_SIZE * sizeof(int));
  for (size_t i = 0; i < INT_TEST_ARRAY_SIZE; i++) {
    test_array[i] = INT_TEST_ARRAY[i];
  }
  return test_array;
}

int **get_int_p_array() {
  int **test_array = malloc(INT_TEST_ARRAY_SIZE * sizeof(int *));
  for (size_t i = 0; i < INT_TEST_ARRAY_SIZE; i++) {
    test_array[i] = (int *)&INT_TEST_ARRAY[i];
  }
  return test_array;
}

char **get_str_array() {
  char **test_array = malloc(STR_TEST_ARRAY_SIZE * sizeof(char *));
  for (size_t i = 0; i < STR_TEST_ARRAY_SIZE; i++) {
    test_array[i] = (char *)STR_TEST_ARRAY[i];
  }
  return test_array;
}

// Freers for each of the array types
void int_arr_free(int *arr, size_t arr_size) { free(arr); }

void str_arr_free(char **arr, size_t arr_size) {
  for (int i = 0; i < arr_size; i++) {
    free(arr[i]);
  }
  free(arr);
}

void int_p_arr_free(int **arr, size_t arr_size) {
  for (int i = 0; i < arr_size; i++) {
    free(arr[i]);
  }
  free(arr);
}

// Functions for generating random arrays of a given type
void rand_str(char *dest, size_t length) {
  char charset[] = "0123456789"
                   "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  while (length-- > 0) {
    size_t index = (double)rand() / RAND_MAX * (sizeof charset - 1);
    *dest++ = charset[index];
  }
  *dest = '\0';
}

int *get_random_int_array(const size_t size) {
  int *test_array = malloc(size * sizeof(int));
  for (size_t i = 0; i < size; i++) {
    test_array[i] = rand() - RAND_MAX / 2;
  }
  return test_array;
}

int **get_random_int_p_array(const size_t size) {
  int **test_array = malloc(size * sizeof(int *));
  for (size_t i = 0; i < size; i++) {
    int *a = malloc(sizeof(int));
    *a = rand();
    test_array[i] = (int *)a;
  }
  return test_array;
}

char **get_random_str_array(const size_t size) {
  char **test_array = malloc(size * sizeof(char *));
  int strlength;
  for (size_t i = 0; i < size; i++) {
    strlength = (rand() % 10 + 1);
    char *str = malloc(sizeof(char) * (strlength + 1));
    rand_str(str, strlength);
    test_array[i] = str;
  }
  return test_array;
}

// Helper functions for checking whether a given array is sorted
// note that int[] is NOT of type void *[] so a seperate function is needed
bool is_sorted_int_array(int *arr, size_t low, size_t high,
                         int_comparator_t comp) {
  if (high - low == 1 || high - low == 0) {
    return true;
  }
  for (size_t i = low + 1; i < high; i++) {
    if (comp(arr[i - 1], arr[i]) > 0) {
      return false;
    }
  }
  return true;
}

bool is_sorted(void *arr[], size_t low, size_t high, comparator_t comp) {
  if (high - low == 1 || high - low == 0) {
    return true;
  }
  for (size_t i = low + 1; i < high; i++) {
    if (comp(arr[i - 1], arr[i]) > 0) {
      return false;
    }
  }
  return true;
}

// Tests for random arrays
void test_ascending_int_sort_whole(int *arr, size_t arr_size, size_t size) {
  ascending_int_sort_whole(arr, size);
  assert(is_sorted_int_array(arr, 0, size, int_asc));
}

void test_ascending_int_sort(int *arr, size_t arr_size, size_t lo, size_t hi) {
  ascending_int_sort(arr, lo, hi);
  assert(is_sorted_int_array(arr, lo, hi, int_asc));
  if (hi < arr_size) {
    assert(!is_sorted_int_array(
        arr, hi, arr_size,
        int_asc)); // Make sure it only sorts between lo and hi
  }
}

void test_descending_int_sort(int *arr, size_t arr_size, size_t lo, size_t hi) {
  descending_int_sort(arr, lo, hi);
  assert(is_sorted_int_array(arr, lo, hi, int_desc));
  if (hi < arr_size) {
    assert(!is_sorted_int_array(
        arr, hi, arr_size,
        int_desc)); // Make sure it only sorts between lo and hi
  }
}

void test_int_sort(int *arr, size_t arr_size, size_t lo, size_t hi,
                   int_comparator_t compare) {
  int_sort(arr, lo, hi, compare);
  assert(is_sorted_int_array(arr, lo, hi, compare));
}

void test_string_sort(char *arr[], size_t arr_size, size_t lo, size_t hi,
                      string_comparator_t compare) {
  string_sort(arr, lo, hi, compare);
  assert(is_sorted((void *)arr, lo, hi, (comparator_t)compare));
}

void test_sort(void *arr[], size_t arr_size, size_t lo, size_t hi,
               comparator_t compare) {
  sort(arr, lo, hi, compare);
  assert(is_sorted(arr, lo, hi, compare));
}

typedef char *str;
typedef int *int_p;

void assert_test_int_arrays_equal(const int arr1[], int arr2[], size_t size) {
  for (int i = 1; i < size; i++) {
    if (arr1[i] != arr2[i]) {
      printf("\nFAILED TEST ascending_int_sort_whole\n");
      printf("Before sorting: ");
      print_int_array(size, (int *) INT_TEST_ARRAY);
      printf("After sorting: ");
      print_int_array(size, arr2);
      printf("Expected sorted array: ");
      print_int_array(size, (int *) arr1);
      free(arr2);
      assert(false);
    }
  }
}

void assert_test_str_arrays_equal(const char *arr1[], char *arr2[], size_t size) {
  for (int i = 1; i < size; i++) {
    if (strcmp(arr1[i], arr2[i]) != 0) {
      printf("\nFAILED TEST string_sort\n");
      printf("Before sorting: ");
      print_str_array(size, (char **) STR_TEST_ARRAY);
      printf("After sorting: ");
      print_str_array(size, arr2);
      printf("Expected sorted array: ");
      print_str_array(size, (char **) arr1);
      free(arr2);
      assert(false);
    }
  }
}

// Actual tests here
void sanity_test_int_sort() {
  int *int_arr = get_int_array();
  ascending_int_sort_whole(int_arr, INT_TEST_ARRAY_SIZE);
  assert_test_int_arrays_equal(INT_TEST_ARRAY_SORTED, int_arr, INT_TEST_ARRAY_SIZE);
  free(int_arr);
}

void sanity_test_str_sort() {
  char **str_arr = get_str_array();
  string_sort(str_arr, 0, STR_TEST_ARRAY_SIZE, strcmp);
  assert_test_str_arrays_equal(STR_TEST_ARRAY_SORTED, str_arr, STR_TEST_ARRAY_SIZE);
  free(str_arr);
}

int main(int argc, char *argv[]) {
  srand(0);
  sanity_test_int_sort();
  DO_RANDOM_TEST(int, ascending_int_sort_whole, INT_TEST_ARRAY_SIZE,
                 INT_TEST_ARRAY_SIZE);
  DO_RANDOM_TEST(int, ascending_int_sort_whole, 1, 1);

  DO_RANDOM_TEST(int, ascending_int_sort, INT_TEST_ARRAY_SIZE, 0,
                 INT_TEST_ARRAY_SIZE);
  DO_RANDOM_TEST(int, ascending_int_sort, INT_TEST_ARRAY_SIZE, 2, 5);
  DO_RANDOM_TEST(int, ascending_int_sort, INT_TEST_ARRAY_SIZE, 0, 4);

  DO_RANDOM_TEST(int, descending_int_sort, INT_TEST_ARRAY_SIZE, 0,
                 INT_TEST_ARRAY_SIZE);
  DO_RANDOM_TEST(int, descending_int_sort, INT_TEST_ARRAY_SIZE, 2, 5);
  DO_RANDOM_TEST(int, descending_int_sort, INT_TEST_ARRAY_SIZE, 0, 4);

  DO_RANDOM_TEST(int, int_sort, INT_TEST_ARRAY_SIZE, 0, INT_TEST_ARRAY_SIZE,
                 int_asc);
  DO_RANDOM_TEST(int, int_sort, INT_TEST_ARRAY_SIZE, 0, INT_TEST_ARRAY_SIZE,
                 int_desc);
  DO_RANDOM_TEST(int, int_sort, 1, 0, 1, int_asc);

  sanity_test_str_sort();
  DO_RANDOM_TEST(str, string_sort, STR_TEST_ARRAY_SIZE, 0, STR_TEST_ARRAY_SIZE,
                 strcmp);

  DO_RANDOM_TEST(int_p, sort, INT_TEST_ARRAY_SIZE, 0, INT_TEST_ARRAY_SIZE,
                 (comparator_t)int_p_asc);
  DO_RANDOM_TEST(int_p, sort, INT_TEST_ARRAY_SIZE, 0, INT_TEST_ARRAY_SIZE,
                 (comparator_t)int_p_desc);
  DO_RANDOM_TEST(str, sort, STR_TEST_ARRAY_SIZE, 0, STR_TEST_ARRAY_SIZE,
                 (comparator_t)strcmp);

  puts("lab03-tests PASS");
}
