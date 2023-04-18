#include "body.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include <stdlib.h>

typedef struct body {
  double mass;
  double rotation;
  list_t *shape;
  rgb_color_t color;
  vector_t velocity;
  vector_t center;
  double angular_vel;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *main_body = malloc(sizeof(body_t));
  main_body->shape = shape;
  main_body->mass = mass;
  main_body->color = color;
  main_body->center = polygon_centroid(main_body->shape);
  main_body->rotation = 0.0;
  main_body->velocity = VEC_ZERO;
  main_body->angular_vel = 0.0;
  return main_body;
}

void body_free(body_t *body) {
  list_free(body->shape);
  free(body);
}

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

vector_t body_get_centroid(body_t *body) { return body->center; }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

double body_get_rotation(body_t *body) { return body->rotation; }

double body_get_angular_vel(body_t *body) { return body->angular_vel; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

void body_set_color(body_t *body, rgb_color_t color) { body->color = color; }

void body_set_centroid(body_t *body, vector_t x) {
  polygon_translate(body->shape, vec_subtract(x, body->center));
  body->center = x;
}

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, body->rotation - angle, body->center);
  body->rotation = angle;
}

void body_set_angular_vel(body_t *body, double velocity) {
  body->angular_vel = velocity;
}

void body_tick(body_t *body, double dt) {
  body_set_centroid(body,
                    vec_add(vec_multiply(dt, body->velocity), body->center));
  body_set_rotation(body, body->rotation + (body->angular_vel * dt));
}