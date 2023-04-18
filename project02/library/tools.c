#include "tools.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#define PI (3.14159265358979323846)

const vector_t MIN_WIN = {0.0, 0.0};
const vector_t MAX_WIN = {1000.0, 500.0};
const double VELOCITY_X = 100;
const double ACCELERATION_Y = -500;
const vector_t ROTATION_BOUNDS = {1, 5};

// star_list_const
const double RESIZE_FACTOR = 2.0;

int r_sign() { return rand() % 2 == 0 ? 1 : -1; }
double r_double() { return rand() * 1.0 / RAND_MAX; }
int r_int(int min, int max) { return min + rand() % (max - min + 1); }
color_t r_color() { return (color_t){r_double(), r_double(), r_double()}; }

background *make_background(color_t color) {
  background *b = malloc(sizeof(background));
  b->color = color;
  b->points = vec_list_init(4);
  vector_t *temp = malloc(sizeof(vector_t));
  *temp = (vector_t){MIN_WIN.x, MIN_WIN.y};
  vec_list_add(b->points, temp);
  temp = malloc(sizeof(vector_t));
  *temp = (vector_t){MIN_WIN.x, MAX_WIN.y};
  vec_list_add(b->points, temp);
  temp = malloc(sizeof(vector_t));
  *temp = (vector_t){MAX_WIN.x, MAX_WIN.y};
  vec_list_add(b->points, temp);
  temp = malloc(sizeof(vector_t));
  *temp = (vector_t){MAX_WIN.x, MIN_WIN.y};
  vec_list_add(b->points, temp);
  return b;
}

star *make_star(size_t sides, double inner_r, double outer_r, double angle,
                vector_t start_pos) {
  assert(sides > 1);
  assert(outer_r > inner_r && inner_r > 0);
  assert(start_pos.x > MIN_WIN.x && start_pos.x < MAX_WIN.x);
  assert(start_pos.y > MIN_WIN.y && start_pos.y < MAX_WIN.y);

  star *s = malloc(sizeof(star));
  s->sides = sides;
  s->color = r_color();
  s->elasticity = r_double() * 0.15 + 0.85;
  s->velocity = (vector_t){VELOCITY_X, 0};
  s->rotation = r_sign() * r_int(ROTATION_BOUNDS.x, ROTATION_BOUNDS.y);
  s->points = vec_list_init(2 * sides);
  for (size_t i = 0; i < sides; i++) {
    vector_t *temp = malloc(sizeof(vector_t));
    double temp_angle = (i * 2.0 * PI) / sides;
    temp->x = outer_r * cos(temp_angle) + start_pos.x;
    temp->y = outer_r * sin(temp_angle) + start_pos.y;
    vec_list_add(s->points, temp);
    temp = malloc(sizeof(vector_t));
    temp_angle += PI / sides;
    temp->x = inner_r * cos(temp_angle) + start_pos.x;
    temp->y = inner_r * sin(temp_angle) + start_pos.y;
    vec_list_add(s->points, temp);
  }
  return s;
}

void star_update(star *s, double dt) {
  // updates position and velocity
  double update_x = s->velocity.x * dt;
  double update_y = s->velocity.y * dt + 0.5 * ACCELERATION_Y * dt * dt;
  vector_t update_pos = (vector_t){update_x, update_y};
  s->velocity = (vector_t){s->velocity.x, s->velocity.y + ACCELERATION_Y * dt};
  polygon_translate(s->points, update_pos);
  polygon_rotate(s->points, dt * s->rotation, polygon_centroid(s->points));
  // adjusts for collision
  for (size_t i = 0; i < s->sides * 2; i += 2) {
    if (vec_list_get(s->points, i)->y <= MIN_WIN.y + 2) {
      s->velocity =
          (vector_t){s->velocity.x, fabs(s->elasticity * s->velocity.y)};
      break;
    }
  }
}

int star_check_remove(star *s) {
  for (size_t i = 0; i < s->sides * 2; i += 2) {
    if (vec_list_get(s->points, i)->x < MAX_WIN.x + 2) {
      return 0;
    }
  }
  return 1;
}

void free_star(star *s) {
  vec_list_free(s->points);
  free(s);
}
void free_background(background *bg) {
  vec_list_free(bg->points);
  free(bg);
}

typedef struct shape_list {
  size_t size;
  size_t capacity;
  star **stars;
} star_list;

star_list *star_list_init(size_t initial_capacity) {
  assert(initial_capacity > 0);
  star_list *lst = malloc(sizeof(star_list));
  lst->size = 0;
  lst->capacity = initial_capacity;
  lst->stars = malloc(initial_capacity * sizeof(star *));
  for (size_t i = 0; i < initial_capacity; i++) {
    lst->stars[i] = NULL;
  }
  return lst;
}

size_t star_list_size(star_list *lst) { return lst->size; }

// returns null if no value at index
star *star_list_get(star_list *lst, size_t index) {
  assert(index < lst->size);
  return lst->stars[index];
}

void star_list_free(star_list *lst) {
  for (size_t i = 0; i < lst->size; i++) {
    free_star(lst->stars[i]);
  }
  free(lst->stars);
  free(lst);
}

void star_list_resize(star_list *lst) {
  lst->capacity = (size_t)floor(lst->capacity * RESIZE_FACTOR);
  star **old_stars = lst->stars;
  lst->stars = malloc(lst->capacity * sizeof(star *));
  for (size_t i = 0; i < lst->size; i++) {
    lst->stars[i] = old_stars[i];
  }
  for (size_t i = lst->size; i < lst->capacity; i++) {
    lst->stars[i] = NULL;
  }
  free(old_stars);
}

void star_list_add(star_list *lst, star *value) {
  assert(value != NULL);
  if (lst->size == lst->capacity) {
    star_list_resize(lst);
  }
  lst->stars[lst->size] = value;
  lst->size++;
}

star *star_list_remove(star_list *lst, size_t index) {
  assert(index < lst->size);
  star *out = lst->stars[index];
  for (size_t i = index; i < lst->size - 1; i++) {
    lst->stars[i] = lst->stars[i + 1];
  }
  lst->stars[lst->size - 1] = NULL;
  lst->size--;
  return out;
}
