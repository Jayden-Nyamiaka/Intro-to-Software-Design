#include "list.h"
#include <assert.h>
#include <stdlib.h>

const double RESIZE_FACTOR = 2.0;

typedef struct list {
  void **data;
  size_t size;
  size_t capacity;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *list = malloc(sizeof(list_t));
  list->data = malloc(initial_size * sizeof(void *));
  list->capacity = initial_size;
  list->freer = freer;
  list->size = 0;
  assert(list->data != NULL);
  return list;
}

void list_free(list_t *list) {
  for (int i = 0; i < list->size; i++) {
      list->freer(list->data[i]);
  } 
  free(list->data);
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size);
  return list->data[index];
}

void *list_remove(list_t *list, size_t index) {
  void *removed = list_get(list, index);
  for (int i = index; i < list->size - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->data[list->size - 1] = NULL;
  list->size--;
  return removed;
}

void list_resize(list_t *list) {
  void **resized = malloc(sizeof(void *) * list->capacity * RESIZE_FACTOR);
  for (int i = 0; i < list->size; i++) {
    resized[i] = list->data[i];
  }
  list->capacity *= RESIZE_FACTOR;
  free(list->data);
  list->data = resized;
}

void list_add(list_t *list, void *value) {
  if (list->size == list->capacity) {
    list_resize(list);
    assert(list->size != list->capacity);
  }
  assert(value != NULL);
  list->data[list->size] = value;
  list->size++;
}