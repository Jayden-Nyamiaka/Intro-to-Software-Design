#include "state.h"
#include <assert.h>
#include <time.h>

const int STARTING_NUMBER_OF_SIDES = 3;
const int STAR_TIME_INTERVAL = 2;
const double INNER_RADIUS = 15;
const double OUTER_RADIUS = 30;
const double ACCELERATION_Y = -500;
const double VELOCITY_X = 100;
const double ROTATION_OF_STARS = 3;
const rgb_color_t BACKGROUND_COLOR = {1, 1, 1};

typedef struct state {
  size_t sides;
  double time_since_star;
  scene_t *scene;
} state_t;

state_t *emscripten_init() {
  srand(time(NULL));
  state_t *state = malloc(sizeof(state_t));
  state->sides = STARTING_NUMBER_OF_SIDES;
  state->time_since_star = STAR_TIME_INTERVAL;
  state->scene = scene_init();
  list_t *background = make_rectangle(VEC_ZERO, vec_multiply(2, WINDOW_CENTER));
  body_t *bg = body_init(background, 0, BACKGROUND_COLOR);
  scene_add_body(state->scene, bg);
  sdl_init(VEC_ZERO, vec_multiply(2, WINDOW_CENTER));
  return state;
}

void check_collision(body_t *body) {
  list_t *shape = body_get_shape(body);
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t *vec = list_get(shape, i);
    vector_t vel = body_get_velocity(body);

    if (vec->y <= VEC_ZERO.y && vel.y < 0) {
      vel.y *= -(r_double(0, .1) + .8);
      body_set_velocity(body, vel);
      break;
    }
  }
  list_free(shape);
}

void star_update(body_t *s, double dt) {
  vector_t v = body_get_velocity(s);
  body_tick(s, dt);
  body_set_velocity(s, vec_add(v, (vector_t){0, ACCELERATION_Y * dt}));
  check_collision(s);
}

int star_check_remove(body_t *s) {
  list_t *shape = body_get_shape(s);
  int in_screen = 1;
  for (size_t i = 0; i < list_size(shape); i += 2) {
    if (((vector_t *)list_get(shape, i))->x < WINDOW_WIDTH + 2) {
      in_screen = 0;
    }
  }
  list_free(shape);
  return in_screen;
}

void add_star(state_t *state, double dt) {
  state->time_since_star += dt;
  if (state->time_since_star > STAR_TIME_INTERVAL) {
    vector_t start_pos = {OUTER_RADIUS, WINDOW_HEIGHT - OUTER_RADIUS};
    body_t *star = body_init(
        make_star(state->sides, INNER_RADIUS, OUTER_RADIUS, start_pos, 0), 5,
        r_color());
    body_set_velocity(star, (vector_t){VELOCITY_X, 0});
    body_set_angular_vel(star, ROTATION_OF_STARS);
    scene_add_body(state->scene, star);
    state->sides++;
    state->time_since_star = 0;
  }
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  add_star(state, dt);

  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    star_update(scene_get_body(state->scene, i), dt);
  }
  if (star_check_remove(scene_get_body(state->scene, 0))) {
    // The furthest star is always the first one in scene. Only need to check index 0.
    scene_remove_body(state->scene, 0);
  }
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
