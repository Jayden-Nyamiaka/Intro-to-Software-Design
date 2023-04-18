#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = {0.0, 0.0};

vector_t vec_add(vector_t v1, vector_t v2) {
  v1.x += v2.x;
  v1.y += v2.y;
  return v1;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;
  return v1;
}

vector_t vec_negate(vector_t v) {
  v.x *= -1;
  v.y *= -1;
  return v;
}

vector_t vec_multiply(double scalar, vector_t v) {
  v.x *= scalar;
  v.y *= scalar;
  return v;
}

double vec_dot(vector_t v1, vector_t v2) {
  return (double)((v1.x * v2.x) + (v1.y * v2.y));
}

double vec_cross(vector_t v1, vector_t v2) {
  return (double)((v1.x * v2.y) - (v1.y * v2.x));
}

vector_t vec_rotate(vector_t v, double angle) {
  double oldX = v.x;
  double cosine = cos(angle);
  double sine = sin(angle);
  v.x = v.x * cosine - v.y * sine;
  v.y = oldX * sine + v.y * cosine;
  return v;
}