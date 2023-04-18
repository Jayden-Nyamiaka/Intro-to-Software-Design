#include "collision.h"
#include "list.h"
#include "polygon.h"
#include <math.h>

// returns overlap on axis (if no overlap (has separating axis) --> returns 0)

// double compute_axis_overlap(list_t *shape1, list_t *shape2,
//                             vector_t unit_axis) {
//   // finds min and max of shape1
//   vector_t v = *(vector_t *)list_get(shape1, 0);
//   double min1 = vec_dot(v, unit_axis);
//   double max1 = min1;
//   for (size_t i = 1; i < list_size(shape1); i++) {
//     v = *(vector_t *)list_get(shape1, i);
//     double projection = vec_dot(v, unit_axis);
//     if (projection < min1) {
//       min1 = projection;
//     } else if (projection > max1) {
//       max1 = projection;
//     }
//   }
//   // finds min and max of shape2
//   v = *(vector_t *)list_get(shape2, 0);
//   double min2 = vec_dot(v, unit_axis);
//   double max2 = min2;
//   for (size_t i = 1; i < list_size(shape2); i++) {
//     v = *(vector_t *)list_get(shape2, i);
//     double projection = vec_dot(v, unit_axis);
//     if (projection < min2) {
//       min2 = projection;
//     } else if (projection > max2) {
//       max2 = projection;
//     }
//   }
//   // compares mins and maxes
//   // touching (0 overlap but still colliding)
//   if (max1 == min2 || max2 == min1) {
//     return 0;
//   }
//   // not colliding at all
//   if (max1 < min2 || max2 < min1) {
//     return -0;
//   }
//   // colliding with overlap, return smaller difference
//   if (max1 - min2 < max2 - min1) {
//     return max1 - min2;
//   }
//   return -1 * (max2 - min1);
// }

// collision_info_t find_collision(list_t *shape1, list_t *shape2) {
//   double min_overlap = -1;
//   vector_t min_overlap_axis;
//   vector_t v1 = *(vector_t *)list_get(shape1, list_size(shape1) - 1);
//   for (size_t i = 0; i < list_size(shape1); i++) {
//     vector_t v2 = *(vector_t *)list_get(shape1, i);
//     vector_t unit_axis = vec_perpendicular_unit_axis(v1, v2);
//     double overlap = compute_axis_overlap(shape1, shape2, unit_axis);
//     if (overlap == -0) {
//       return (collision_info_t){false, VEC_ZERO};
//     }
//     if (min_overlap == -1) {
//       min_overlap = overlap;
//       min_overlap_axis = unit_axis;
//     } else {
//       if (fabs(overlap) < min_overlap) {
//         min_overlap = fabs(overlap);
//         if (overlap < 0) {
//           min_overlap_axis = vec_negate(unit_axis);
//         } else {
//           min_overlap_axis = unit_axis;
//         }
//       }
//     }
//     v1 = v2;
//   }
//   v1 = *(vector_t *)list_get(shape2, list_size(shape2) - 1);
//   for (size_t i = 0; i < list_size(shape2); i++) {
//     vector_t v2 = *(vector_t *)list_get(shape2, i);
//     vector_t unit_axis = vec_perpendicular_unit_axis(v1, v2);
//     double overlap = compute_axis_overlap(shape1, shape2, unit_axis);
//     if (overlap == -0) {
//       return (collision_info_t){false, VEC_ZERO};
//     }
//     if (fabs(overlap) < min_overlap) {
//       min_overlap = fabs(overlap);
//       if (overlap < 0) {
//         min_overlap_axis = vec_negate(unit_axis);
//       } else {
//         min_overlap_axis = unit_axis;
//       }
//     }
//     v1 = v2;
//   }
//   return (collision_info_t){true, min_overlap_axis};
// }

double compute_axis_overlap(list_t *shape1, list_t *shape2,
                            vector_t unit_axis) {
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
  // compares mins and maxes
  // touching (0 overlap but still colliding)
  if (max1 == min2 || max2 == min1) {
    return 0;
  }
  // not colliding at all
  if (max1 < min2 || max2 < min1) {
    return -1;
  }
  // colliding with overlap, return smaller difference
  if (max1 - min2 < max2 - min1) {
    return max1 - min2;
  }
  return max2 - min1;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  double min_overlap = -1;
  vector_t min_overlap_axis;
  vector_t unit_axis;

  vector_t v1 = *(vector_t *)list_get(shape1, list_size(shape1) - 1);
  for (size_t i = 0; i < list_size(shape1); i++) {
    vector_t v2 = *(vector_t *)list_get(shape1, i);
    unit_axis = vec_perpendicular_unit_axis(v1, v2);
    // if (vec_distance(centroid2, v1) < vec_distance(centroid2, v2)) {
    //   unit_axis = vec_perpendicular_unit_axis(v2, v1);
    // } else {
    //   unit_axis = vec_perpendicular_unit_axis(v1, v2);
    // }
    double overlap = compute_axis_overlap(shape1, shape2, unit_axis);
    if (overlap == -1) {
      return (collision_info_t){false, VEC_ZERO};
    }
    if (min_overlap == -1) {
      min_overlap = overlap;
      min_overlap_axis = unit_axis;
    } else {
      if (overlap < min_overlap) {
        min_overlap = overlap;
        min_overlap_axis = unit_axis;
      }
    }
    v1 = v2;
  }

  v1 = *(vector_t *)list_get(shape2, list_size(shape2) - 1);
  for (size_t i = 0; i < list_size(shape2); i++) {
    vector_t v2 = *(vector_t *)list_get(shape2, i);
    unit_axis = vec_perpendicular_unit_axis(v1, v2);
    // if (vec_distance(centroid1, v1) < vec_distance(centroid1, v2)) {
    //   unit_axis = vec_perpendicular_unit_axis(v2, v1);
    // } else {
    //   unit_axis = vec_perpendicular_unit_axis(v1, v2);
    // }
    double overlap = compute_axis_overlap(shape1, shape2, unit_axis);
    if (overlap == -1) {
      return (collision_info_t){false, VEC_ZERO};
    }
    if (overlap < min_overlap) {
      min_overlap = overlap;
      min_overlap_axis = unit_axis;
    }
    v1 = v2;
  }
  return (collision_info_t){true, min_overlap_axis};
}