#include "vec_list.h"
#include <assert.h>
#include <stdlib.h>

typedef struct vec_list {
  size_t size;
  size_t capacity;
  vector_t **data;
} vec_list_t;

// const double CAPACITY_INCREASE_FACTOR = 2.0;

vec_list_t *vec_list_init(size_t initial_size) {
  vec_list_t *lst = malloc(sizeof(vec_list_t));
  lst->size = 0;
  lst->capacity = initial_size;
  lst->data = malloc(initial_size * sizeof(vector_t *));
  return lst;
}

void vec_list_free(vec_list_t *list) {
  for (size_t i = 0; i < list->size; i++) {
    free(list->data[i]);
  }
  free(list->data);
  free(list);
}

size_t vec_list_size(vec_list_t *list) { return list->size; }

vector_t *vec_list_get(vec_list_t *list, size_t index) {
  assert(index >= 0 && index < list->size);
  return list->data[index];
}

void vec_list_add(vec_list_t *list, vector_t *value) {
  assert(value != NULL && list->size < list->capacity);
  /*if (list->size == list->capacity) {
      vector_t **newData = malloc(CAPACITY_INCREASE_FACTOR * list->capacity *
  sizeof(vector_t *)); for (size_t i = 0; i < list->size; i++) { newData[i] =
  list->data[i];
      }
      list->data = newData;
  }
  */
  list->data[list->size] = value;
  list->size++;
}

vector_t *vec_list_remove(vec_list_t *list) {
  assert(list->size > 0);
  list->size--;
  return list->data[list->size];
}
