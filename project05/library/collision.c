#include "collision.h"
#include "list.h"
#include "vector.h"
#include <stdlib.h>

bool has_separating_axis(list_t *shape1, list_t *shape2, vector_t point1,
                         vector_t point2) {
  vector_t unit_axis = vec_perpendicular_unit_axis(point1, point2);
  // finds min and max of shape1
  vector_t v = *(vector_t *)list_get(shape1, 0);
  double min1 = vec_dot(v, unit_axis);
  double max1 = min1;
  for (size_t i = 1; i < list_size(shape1); i++) {
    v = *(vector_t *)list_get(shape1, i);
    double projection = vec_dot(v, unit_axis);
    if (projection < min1) {
      min1 = projection;
    } else if (projection > max1) {
      max1 = projection;
    }
  }
  // finds min and max of shape2
  v = *(vector_t *)list_get(shape2, 0);
  double min2 = vec_dot(v, unit_axis);
  double max2 = min2;
  for (size_t i = 1; i < list_size(shape2); i++) {
    v = *(vector_t *)list_get(shape2, i);
    double projection = vec_dot(v, unit_axis);
    if (projection < min2) {
      min2 = projection;
    } else if (projection > max2) {
      max2 = projection;
    }
  }
  // compares mins and maxes to check if separating axis exists
  if (max1 < min2 || max2 < min1) {
    return true;
  }
  return false;
}

bool find_collision(list_t *shape1, list_t *shape2) {
  vector_t v1 = *(vector_t *)list_get(shape1, list_size(shape1) - 1);
  for (size_t i = 0; i < list_size(shape1); i++) {
    vector_t v2 = *(vector_t *)list_get(shape1, i);
    if (has_separating_axis(shape1, shape2, v1, v2)) {
      return false;
    }
    v1 = v2;
  }
  v1 = *(vector_t *)list_get(shape2, list_size(shape2) - 1);
  for (size_t i = 0; i < list_size(shape2); i++) {
    vector_t v2 = *(vector_t *)list_get(shape2, i);
    if (has_separating_axis(shape1, shape2, v1, v2)) {
      return false;
    }
    v1 = v2;
  }
  return true;
}