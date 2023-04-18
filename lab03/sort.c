#include "sort.h"
#include "comparator.h"

// Selection Sort Used

void swap_int(int arr[], size_t i, size_t j) {
  if (i == j) {
    return;
  }
  arr[i] = arr[i] + arr[j];
  arr[j] = arr[i] - arr[j];
  arr[i] = arr[i] - arr[j];
  return;
}

void swap_pointer(void *arr[], size_t i, size_t j) {
  if (i == j) {
    return;
  }
  void *temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
  return;
}

void ascending_int_sort_whole(int arr[], size_t nelements) {
  ascending_int_sort(arr, 0, nelements);
  return;
}

void ascending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, int_asc);
  return;
}

void descending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, int_desc);
  return;
}

void int_sort(int arr[], size_t lo, size_t hi, int_comparator_t compare) {
  size_t most_index;
  for (size_t i = lo; i < hi; i++) {
    most_index = i;
    for (size_t j = i + 1; j < hi; j++) {
      if (compare(arr[most_index], arr[j]) > 0) {
        most_index = j;
      }
    }
    swap_int(arr, most_index, i);
  }
  return;
}

void string_sort(char *arr[], size_t lo, size_t hi,
                 string_comparator_t compare) {
  sort((void **)arr, lo, hi, (comparator_t)compare);
  return;
}

void sort(void *arr[], size_t lo, size_t hi, comparator_t compare) {
  size_t most_index;
  for (size_t i = lo; i < hi; i++) {
    most_index = i;
    for (size_t j = i + 1; j < hi; j++) {
      if (compare(arr[most_index], arr[j]) > 0) {
        most_index = j;
      }
    }
    swap_pointer(arr, most_index, i);
  }
  return;
}
