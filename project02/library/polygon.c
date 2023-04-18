#include "polygon.h"
#include "vec_list.h"
#include "vector.h"
#include <stdlib.h>

double polygon_area(vec_list_t *polygon) {
  double total = 0;
  int size = vec_list_size(polygon);
  for (int i = 0; i < size; i++) {
    total += vec_cross(*vec_list_get(polygon, i),
                       *vec_list_get(polygon, (i + 1) % size));
  }
  total /= 2;
  return total;
}

vector_t polygon_centroid(vec_list_t *polygon) {
  vector_t v;
  v.x = 0;
  v.y = 0;
  int size = vec_list_size(polygon);
  for (int i = 0; i < size; i++) {
    vector_t vec1 = *vec_list_get(polygon, i);
    vector_t vec2 = *vec_list_get(polygon, (i + 1) % size);
    double cross = vec_cross(vec1, vec2);
    v.x += (vec1.x + vec2.x) * cross;
    v.y += (vec1.y + vec2.y) * cross;
  }
  v = vec_multiply((1 / (6 * polygon_area(polygon))), v);
  return v;
}

void polygon_translate(vec_list_t *polygon, vector_t translation) {
  int size = vec_list_size(polygon);
  for (int i = 0; i < size; i++) {
    vector_t *vec = vec_list_get(polygon, i);
    *vec = vec_add(*vec, translation);
  }
}

void polygon_rotate(vec_list_t *polygon, double angle, vector_t point) {
  int size = vec_list_size(polygon);
  for (int i = 0; i < size; i++) {
    vector_t *vec = vec_list_get(polygon, i);
    *vec = vec_subtract(*vec, point);
    *vec = vec_rotate(*vec, angle);
    *vec = vec_add(*vec, point);
  }
}
