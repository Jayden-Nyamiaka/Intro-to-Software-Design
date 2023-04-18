#include "vec_list.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>

typedef struct vec_list {
  vector_t **arr;
  size_t size;
  size_t capacity;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size) {
  vec_list_t *list = malloc(sizeof(vec_list_t));
  list->arr = malloc(initial_size * sizeof(vector_t *));
  list->size = 0;
  list->capacity = initial_size;
  return list;
}

void vec_list_free(vec_list_t *list) {
  for (size_t i = 0; i < list->size; i++) {
    free(list->arr[i]);
  }
  free(list->arr);
  free(list);
}

size_t vec_list_size(vec_list_t *list) { return list->size; }

vector_t *vec_list_get(vec_list_t *list, size_t index) {
  assert(index < list->size);
  return list->arr[index];
}

void vec_list_add(vec_list_t *list, vector_t *value) {
  assert(list->size < list->capacity);
  assert(value != NULL);
  list->arr[list->size] = value;
  list->size++;
}

vector_t *vec_list_remove(vec_list_t *list) {
  assert(list->size > 0);
  vector_t *temp = list->arr[list->size - 1];
  list->size--;
  return temp;
}
