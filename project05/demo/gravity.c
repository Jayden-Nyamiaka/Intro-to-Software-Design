#include "state.h"
#include <assert.h>
#include <time.h>

const int STARTING_NUMBER_OF_SIDES = 3;
const int STAR_TIME_INTERVAL = 2;
const double INNER_RADIUS = 15;
const double OUTER_RADIUS = 30;
const double ACCELERATION_Y = -500;
const double VELOCITY_X = 100;
const double MASS = 50;
const double ROTATION_OF_STARS = 3;
const rgb_color_t BACKGROUND_COLOR = {1, 1, 1};
const vector_t ELASTICITY_BOUNDS = {0.87, 0.97};
const double LITTLE_G_CONSTANT = 1000;

typedef struct state {
  size_t sides;
  double time_since_star;
  scene_t *scene;
} state_t;

void add_star(state_t *state, double dt) {
  state->time_since_star += dt;
  if (state->time_since_star > STAR_TIME_INTERVAL) {
    vector_t start_pos = {OUTER_RADIUS, WINDOW_HEIGHT - OUTER_RADIUS};
    body_t *star = body_init(
        make_star(state->sides, INNER_RADIUS, OUTER_RADIUS, start_pos, 0), MASS,
        r_color());
    body_set_velocity(star, (vector_t){VELOCITY_X, 0});
    body_set_angular_velocity(
        star,
        ROTATION_OF_STARS * r_double(ELASTICITY_BOUNDS.x, ELASTICITY_BOUNDS.y));
    scene_add_body(state->scene, star);
    // adds force (earth gravity) to new star
    list_t *bodies = list_init(1, NULL);
    list_add(bodies, star);
    create_earth_gravity(state->scene, LITTLE_G_CONSTANT, bodies);
    state->sides++;
    state->time_since_star = 0;
  }
}

// Only checks star @ i = 0 (furthest star)
void star_check_remove(scene_t *scene) {
  body_t *last_star = scene_get_body(scene, 0);
  list_t *shape = body_get_shape(last_star);
  int in_screen = 1;
  for (size_t i = 0; i < list_size(shape); i += 2) {
    if (((vector_t *)list_get(shape, i))->x < WINDOW_WIDTH + 2) {
      in_screen = 0;
      break;
    }
  }
  list_free(shape);
  if (in_screen) {
    body_remove(last_star);
  }
  return;
}

void check_collisions(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    list_t *shape = body_get_shape(body);
    for (size_t i = 0; i < list_size(shape); i++) {
      vector_t *vec = list_get(shape, i);
      vector_t vel = body_get_velocity(body);

      if (vec->y <= VEC_ZERO.y && vel.y < 0) {
        double elasticity = body_get_angular_velocity(body) / ROTATION_OF_STARS;
        vel.y *= -1 * elasticity;
        body_set_velocity(body, vel);
        break;
      }
    }
    list_free(shape);
  }
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, vec_multiply(2, WINDOW_CENTER));
  state_t *state = malloc(sizeof(state_t));
  state->sides = STARTING_NUMBER_OF_SIDES;
  state->time_since_star = STAR_TIME_INTERVAL + 1;
  state->scene = scene_init();
  // adds background
  scene_add_body(state->scene,
                 body_init(make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH,
                                                               WINDOW_HEIGHT}),
                           0, BACKGROUND_COLOR));
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  add_star(state, dt);
  scene_tick(state->scene, dt);
  star_check_remove(state->scene);
  check_collisions(state->scene);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
