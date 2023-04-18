#include "state.h"
#include <math.h>


const size_t BODY_COUNT = 100;
const vector_t SPAWN_WINDOW_RANGE = {0.1, 0.9}; 
const vector_t STAR_POINT_RANGE = {5, 8};
const vector_t INNER_RADIUS_RANGE = {5, 20}; 
const vector_t OUTER_TO_INNER_RATIO_RANGE = {1.4, 2.0};
const vector_t MASS_RANGE = {20, 300};
const rgb_color_t BACKGROUND_COLOR = {0, 0, 0};
const double BIG_G_CONSTANT = 500; 

typedef struct state {
  scene_t *scene;
} state_t;

body_t *make_nbody() {
  int points = r_int(STAR_POINT_RANGE.x, STAR_POINT_RANGE.y);
  double inner = r_double(INNER_RADIUS_RANGE.x, INNER_RADIUS_RANGE.y);
  double outer = inner * r_double(OUTER_TO_INNER_RATIO_RANGE.x, OUTER_TO_INNER_RATIO_RANGE.y);
  double center_x = r_double(SPAWN_WINDOW_RANGE.x, SPAWN_WINDOW_RANGE.y) * WINDOW_WIDTH;
  double center_y = r_double(SPAWN_WINDOW_RANGE.x, SPAWN_WINDOW_RANGE.y) * WINDOW_HEIGHT;
  double mass = r_double(MASS_RANGE.x, MASS_RANGE.y);
  rgb_color_t color = r_pastel_color();
  list_t *star = make_star(points, inner, outer, (vector_t) {center_x, center_y}, 0);
  body_t *nbody = body_init(star, mass, color);
  return nbody;
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  // adds background
  scene_add_body(
      state->scene,
      body_init(make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT}), 0, BACKGROUND_COLOR));
  // adds nbodies
  for (size_t i = 0; i < BODY_COUNT; i++) {
    scene_add_body(state->scene, make_nbody());
  }
  // adds all forces (newtonian gravity)
  for (size_t i = 1; i < scene_bodies(state->scene) - 1; i++) {
    for (size_t j = i + 1; j < scene_bodies(state->scene); j++) {
      create_newtonian_gravity(state->scene, BIG_G_CONSTANT, scene_get_body(state->scene, i), scene_get_body(state->scene, j));
    }
  }
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}