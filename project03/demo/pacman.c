#include "state.h"
#include <math.h>

#define RIGHT (0)
#define UP (1.5 * M_PI)
#define LEFT (M_PI)
#define DOWN (0.5 * M_PI)

const double PELLET_SPAWN_TIME = 2;      // in seconds
const size_t INITIAL_PELLET_NUMBER = 20; // 20
const size_t PELLET_INNER_RADIUS = 4;
const size_t PELLET_OUTER_RADIUS = 6;
const size_t PELLET_POINTS = 8;
const size_t PACMAN_RADIUS = 30;
const size_t DOTS_PER_RADIUS = 10;
const double MOUTH_ANGLE_RADIANS = 2 * M_PI * 9 / 36;
const double PACMAN_ACCELERATION = 10;
const rgb_color_t PACMAN_PELLET_COLOR = {1, 1, 0}; // yellow
const rgb_color_t BACKGROUND_COLOR = {0, 0, 0};
const double DRIFT_VELOCITY = 120;

typedef struct state {
  scene_t *scene;
  body_t *background;
  body_t *pacman;
  double time_since_pellet;
} state_t;

// center at list[i] = 0
list_t *draw_pacman(double radius, vector_t center) {
  size_t dots = DOTS_PER_RADIUS * radius;
  list_t *pacman = list_init(dots + 1, free);
  double angle = (M_PI * 2 - MOUTH_ANGLE_RADIANS) / dots;

  vector_t *v = malloc(sizeof(vector_t));
  *v = VEC_ZERO;
  list_add(pacman, v);
  for (int i = 0; i < dots; i++) {
    v = malloc(sizeof(vector_t));
    *v = vec_rotate((vector_t){radius, 0},
                    i * angle + 0.5 * MOUTH_ANGLE_RADIANS);
    list_add(pacman, v);
  }
  polygon_translate(pacman, center);
  return pacman;
}

vector_t pacman_get_center(body_t *pacman) {
  list_t *pacman_shape = body_get_shape(pacman);
  vector_t *centerPointer = (vector_t *) list_get(pacman_shape, 0);
  vector_t centerValue = *centerPointer;
  list_free(pacman_shape);
  return centerValue;
}

body_t *make_pellet() {
  vector_t point =
      (vector_t){(int)r_double(WINDOW_WIDTH * 0.05, WINDOW_WIDTH * 0.95),
                 (int)r_double(WINDOW_HEIGHT * 0.05, WINDOW_HEIGHT * 0.95)};
  list_t *star = make_star(PELLET_POINTS, PELLET_INNER_RADIUS,
                           PELLET_OUTER_RADIUS, point, 0);
  body_t *pellet = body_init(star, 0, PACMAN_PELLET_COLOR);
  return pellet;
}

int is_point_inside_rectangle(vector_t point, vector_t bottom_left,
                              vector_t top_right) {
  if (point.x > bottom_left.x && point.x < top_right.x &&
      point.y > bottom_left.y && point.y < top_right.y) {
    return 1;
  }
  return 0;
}

void add_pellets(state_t *state, double dt) {
  state->time_since_pellet += dt;
  if (state->time_since_pellet > PELLET_SPAWN_TIME) {
    scene_add_body(state->scene, make_pellet());
    state->time_since_pellet = 0;
  }
}

void check_eat_pellets(state_t *state) {
  vector_t center = pacman_get_center(state->pacman);
  double distance = 1.1 * PACMAN_RADIUS * sin(MOUTH_ANGLE_RADIANS);
  vector_t bottom_left =
      (vector_t){center.x - 0.5 * PACMAN_RADIUS, center.y - distance};
  vector_t top_right =
      (vector_t){center.x + 0.5 * PACMAN_RADIUS, center.y + distance};

  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *pellet = scene_get_body(state->scene, i);
    if (pellet == state->pacman || pellet == state->background) {
      continue;
    }
    vector_t pellet_center = body_get_centroid(pellet);
    if (is_point_inside_rectangle(pellet_center, bottom_left, top_right)) {
      scene_remove_body(state->scene, i);
    }
  }
}

void check_offscreen(body_t *pacman) {
  vector_t center = pacman_get_center(pacman);
  if (center.x - PACMAN_RADIUS > WINDOW_WIDTH) {
    body_set_centroid(pacman, (vector_t){PACMAN_RADIUS, center.y});
    center.x = -1 * PACMAN_RADIUS;
  } else if (center.x + PACMAN_RADIUS < 0) {
    body_set_centroid(pacman, (vector_t){WINDOW_WIDTH - PACMAN_RADIUS, center.y});
    center.x = WINDOW_WIDTH + PACMAN_RADIUS;
  }
  if (center.y - PACMAN_RADIUS > WINDOW_HEIGHT) {
    body_set_centroid(pacman, (vector_t){center.x, PACMAN_RADIUS});
  } else if (center.y + PACMAN_RADIUS < 0) {
    body_set_centroid(pacman, (vector_t){center.x, WINDOW_HEIGHT - PACMAN_RADIUS});
  }
}

void pacman_move(body_t *pacman, key_event_type_t type, double held_time, double orientation, vector_t direction) {
  vector_t velocity = body_get_velocity(pacman);
  if (type == KEY_PRESSED) { 
    if (body_get_rotation(pacman) != orientation) {
      velocity = vec_multiply(DRIFT_VELOCITY, direction);
      body_set_rotation(pacman, orientation);
    }
    velocity = vec_add(velocity, vec_multiply(PACMAN_ACCELERATION * held_time, direction));
    body_set_velocity(pacman, velocity);
  } else {
    body_set_rotation(pacman, orientation);
    body_set_velocity(pacman, vec_multiply(DRIFT_VELOCITY, direction));
  }
}

void on_key(char key, key_event_type_t type, double held_time, void *state) {
  body_t *pacman = ((state_t *)state)->pacman;
  switch (key) {
    case RIGHT_ARROW:
      pacman_move(pacman, type, held_time, RIGHT, (vector_t) {1, 0});
      break;
    case UP_ARROW:
      pacman_move(pacman, type, held_time, UP, (vector_t) {0, 1});
      break;
    case LEFT_ARROW:
      pacman_move(pacman, type, held_time, LEFT, (vector_t) {-1, 0});
      break;
    case DOWN_ARROW:
      pacman_move(pacman, type, held_time, DOWN, (vector_t) {0, -1});
      break;
  }
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->time_since_pellet = 0;
  // adds background
  state->background = body_init(make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT}), 0,
                BACKGROUND_COLOR);
  scene_add_body(state->scene, state->background);
  // adds pacman
  state->pacman = body_init(draw_pacman(PACMAN_RADIUS, WINDOW_CENTER), 0, PACMAN_PELLET_COLOR);
  scene_add_body(state->scene, state->pacman);
  // adds initial pellets
  for (size_t i = 0; i < INITIAL_PELLET_NUMBER; i++) {
    scene_add_body(state->scene, make_pellet());
  }
  sdl_on_key((void *)on_key);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  add_pellets(state, dt);
  check_offscreen(state->pacman);
  check_eat_pellets(state);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}