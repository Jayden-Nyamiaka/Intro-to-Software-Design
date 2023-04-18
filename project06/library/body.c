#include "body.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>

typedef struct body {
  double mass;
  list_t *shape;
  rgb_color_t color;
  vector_t center;
  double rotation;
  double angular_velocity;
  vector_t velocity;
  vector_t acceleration;
  vector_t force;
  vector_t impulse;
  void *info;
  free_func_t info_freer;
  bool is_removed;
  bool removable;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = malloc(sizeof(body_t));
  body->shape = shape;
  body->mass = mass;
  body->color = color;
  body->center = polygon_centroid(body->shape);
  body->rotation = 0.0;
  body->angular_velocity = 0.0;
  body->velocity = VEC_ZERO;
  body->acceleration = VEC_ZERO;
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->is_removed = false;
  body->removable = true;
  return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = body_init(shape, mass, color);
  body->info = info;
  body->info_freer = info_freer;
  return body;
}

void body_free(body_t *body) {
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  list_free(body->shape);
  free(body);
}

void *body_get_info(body_t *body) { return body->info; }

list_t *body_get_shape(body_t *body) {
  size_t size = list_size(body->shape);
  list_t *lst = list_init(size, free);
  for (size_t i = 0; i < size; i++) {
    vector_t *temp = malloc(sizeof(vector_t));
    *temp = *(vector_t *)list_get(body->shape, i);
    list_add(lst, temp);
  }
  return lst;
}

double body_get_mass(body_t *body) { return body->mass; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

vector_t body_get_centroid(body_t *body) { return body->center; }

double body_get_rotation(body_t *body) { return body->rotation; }

double body_get_angular_velocity(body_t *body) {
  return body->angular_velocity;
}

vector_t body_get_velocity(body_t *body) { return body->velocity; }

vector_t body_get_acceleration(body_t *body) { return body->acceleration; }

void body_set_color(body_t *body, rgb_color_t color) { body->color = color; }

void body_set_centroid(body_t *body, vector_t x) {
  polygon_translate(body->shape, vec_subtract(x, body->center));
  body->center = x;
}

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, body->rotation - angle, body->center);
  body->rotation = angle;
}

void body_set_angular_velocity(body_t *body, double velocity) {
  body->angular_velocity = velocity;
}

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_acceleration(body_t *body, vector_t v) { body->acceleration = v; }

void body_dilate_x(body_t *body, double factor) {
  polygon_dilate_x(body->shape, factor);
  vector_t new_center = polygon_centroid(body->shape);
  polygon_translate(body->shape, vec_subtract(body->center, new_center));
}

void body_dilate_y(body_t *body, double factor) {
  polygon_dilate_y(body->shape, factor);
  vector_t new_center = polygon_centroid(body->shape);
  polygon_translate(body->shape, vec_subtract(body->center, new_center));
}

void body_dilate(body_t *body, double factor) {
  polygon_dilate(body->shape, factor);
  vector_t new_center = polygon_centroid(body->shape);
  polygon_translate(body->shape, vec_subtract(body->center, new_center));
}

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt) {
  if (body->mass != INFINITY && body->mass != 0) {
    vector_t velocity_change_from_force =
        vec_multiply(dt / body->mass, body->force);
    vector_t velocity_change_from_impulse =
        vec_multiply(1.0 / body->mass, body->impulse);
    vector_t change_in_velocity =
        vec_add(velocity_change_from_force, velocity_change_from_impulse);
    vector_t velocity_after_tick = vec_add(body->velocity, change_in_velocity);
    vector_t average_velocity =
        vec_multiply(0.5, vec_add(body->velocity, velocity_after_tick));
    body_set_velocity(body, velocity_after_tick);
    body_set_acceleration(body, vec_multiply(1.0 / body->mass, body->force));
    body_set_centroid(
        body, vec_add(vec_multiply(dt, average_velocity), body->center));
  } else {
    body_set_centroid(body,
                      vec_add(vec_multiply(dt, body->velocity), body->center));
  }
  body_set_rotation(body, body->rotation + (body->angular_velocity * dt));
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

void body_remove(body_t *body) { body->is_removed = true; }

bool body_is_removed(body_t *body) { return body->is_removed; }

bool body_is_removable(body_t *body) { return body->removable; }

void body_set_removability(body_t *body, bool removable) {
  body->removable = removable;
}