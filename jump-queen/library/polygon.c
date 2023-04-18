#include "polygon.h"
#include "list.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>

double polygon_area(list_t *polygon) {
  double total = 0;
  int size = list_size(polygon);
  for (int i = 0; i < size; i++) {
    total += vec_cross(*(vector_t *)list_get(polygon, i),
                       *(vector_t *)list_get(polygon, (i + 1) % size));
  }
  total /= 2;
  return total;
}

vector_t polygon_centroid(list_t *polygon) {
  vector_t v = VEC_ZERO;
  int size = list_size(polygon);
  for (int i = 0; i < size; i++) {
    vector_t vec1 = *(vector_t *)list_get(polygon, i);
    vector_t vec2 = *(vector_t *)list_get(polygon, (i + 1) % size);
    double cross = vec_cross(vec1, vec2);
    v = vec_add(v, vec_multiply(cross, vec_add(vec1, vec2)));
  }
  v = vec_multiply(1 / (6 * polygon_area(polygon)), v);
  return v;
}

void polygon_translate(list_t *polygon, vector_t translation) {
  for (int i = 0; i < list_size(polygon); i++) {
    vector_t *vec = list_get(polygon, i);
    *vec = vec_add(*vec, translation);
  }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  for (int i = 0; i < list_size(polygon); i++) {
    vector_t *vec = list_get(polygon, i);
    *vec = vec_add(vec_rotate(vec_subtract(*vec, point), angle), point);
  }
}

void polygon_dilate_x(list_t *polygon, double factor) {
  for (int i = 0; i < list_size(polygon); i++) {
    vector_t *vec = list_get(polygon, i);
    *vec = (vector_t){factor * vec->x, vec->y};
  }
}

void polygon_dilate_y(list_t *polygon, double factor) {
  for (int i = 0; i < list_size(polygon); i++) {
    vector_t *vec = list_get(polygon, i);
    *vec = (vector_t){vec->x, factor * vec->y};
  }
}

void polygon_dilate(list_t *polygon, double factor) {
  polygon_dilate_x(polygon, factor);
  polygon_dilate_y(polygon, factor);
}

list_t *make_closed_polygon(double length, int points) {
  list_t *polygon = list_init(points, free);
  double angle = M_PI * 2 / points;
  for (int i = 0; i < points; i++) {
    vector_t *v = malloc(sizeof(vector_t));
    *v = vec_rotate((vector_t){0, length}, angle * i);
    list_add(polygon, v);
  }
  return polygon;
}

list_t *make_star(int points, double inner_radius, double outer_radius,
                  vector_t center, double angle) {
  list_t *star = list_init(points * 2, free);
  list_t *poly = make_closed_polygon(outer_radius, points);
  list_t *poly1 = make_closed_polygon(inner_radius, points);
  polygon_rotate(poly1, M_PI / points, VEC_ZERO);
  for (int i = 0; i < points; i++) {
    list_add(star, list_get(poly, i));
    list_add(star, list_get(poly1, i));
  }
  polygon_rotate(star, angle, VEC_ZERO);
  polygon_translate(star, center);
  return star;
}

list_t *make_rectangle(vector_t corner_one, vector_t corner_two) {
  list_t *rect = list_init(4, free);
  vector_t *v = malloc(sizeof(vector_t));
  *v = (vector_t){corner_two.x, corner_two.y};
  list_add(rect, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t){corner_one.x, corner_two.y};
  list_add(rect, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t){corner_one.x, corner_one.y};
  list_add(rect, v);
  v = malloc(sizeof(vector_t));
  *v = (vector_t){corner_two.x, corner_one.y};
  list_add(rect, v);
  return rect;
}

list_t *make_circle(double radius, double dots_per_radius, vector_t center) {
  list_t *circle = make_closed_polygon(radius, dots_per_radius * radius);
  polygon_translate(circle, center);
  return circle;
}