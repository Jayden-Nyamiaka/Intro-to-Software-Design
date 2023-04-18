#include "polygon.h"
#include <math.h>

double polygon_area(vec_list_t *polygon) {
  double numerator = 0;
  for (size_t i = 0; i < vec_list_size(polygon); i++) {
    vector_t *current = vec_list_get(polygon, i);
    vector_t *next;
    if (i + 1 == vec_list_size(polygon)) {
      next = vec_list_get(polygon, 0);
    } else {
      next = vec_list_get(polygon, i + 1);
    }
    numerator += (current->x * next->y) - (current->y * next->x);
  }
  return fabs(numerator / 2);
}

vector_t polygon_centroid(vec_list_t *polygon) {
  vector_t out = VEC_ZERO;
  for (size_t i = 0; i < vec_list_size(polygon); i++) {
    vector_t *current = vec_list_get(polygon, i);
    vector_t *next;
    if (i + 1 == vec_list_size(polygon)) {
      next = vec_list_get(polygon, 0);
    } else {
      next = vec_list_get(polygon, i + 1);
    }
    double term = (current->x * next->y - next->x * current->y);
    vector_t components = {current->x + next->x, current->y + next->y};
    out = vec_add(out, vec_multiply(term, components));
  }
  return vec_multiply(1.0 / (6 * polygon_area(polygon)), out);
}

void polygon_translate(vec_list_t *polygon, vector_t translation) {
  for (size_t i = 0; i < vec_list_size(polygon); i++) {
    vector_t *current = vec_list_get(polygon, i);
    vector_t temp = vec_add(*current, translation);
    current->x = temp.x;
    current->y = temp.y;
  }
}

void polygon_rotate(vec_list_t *polygon, double angle, vector_t point) {
  for (size_t i = 0; i < vec_list_size(polygon); i++) {
    vector_t *current = vec_list_get(polygon, i);
    vector_t temp = vec_subtract(*current, point);
    temp = vec_rotate(temp, angle);
    temp = vec_add(temp, point);
    current->x = temp.x;
    current->y = temp.y;
  }
}