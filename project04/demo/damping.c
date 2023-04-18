#include "state.h"
#include <math.h>


const size_t BALL_RADIUS = 10; // 10
const double BALL_MASS = 30; // 30
const size_t DOTS_PER_RADIUS = 10;
const double PERIODS_ON_SCREEN = 3;
const rgb_color_t BACKGROUND_COLOR = {1, 1, 1};
const double STIFFNESS_K_CONSTANT = 300;
const double DRAG_GAMMA_CONSTANT = 4;

typedef struct state {
  scene_t *scene;
} state_t;

body_t *make_real_ball(vector_t center) {
  list_t *circle = make_circle(BALL_RADIUS, DOTS_PER_RADIUS, center);
  body_t *ball = body_init(circle, BALL_MASS, r_color());
  return ball;
}

body_t *make_invisiball(vector_t center) {
  list_t *circle = make_circle(0.5, DOTS_PER_RADIUS, center);
  body_t *ball = body_init(circle, INFINITY, BACKGROUND_COLOR);
  return ball;
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  // adds background
  scene_add_body(
      state->scene,
      body_init(make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT}), 0,
                BACKGROUND_COLOR));
  // adds balls
  double angle = (PERIODS_ON_SCREEN * 2*M_PI) / WINDOW_WIDTH;
  for (double x = 0; x < WINDOW_WIDTH; x += 2*BALL_RADIUS) {
    double position_x = x + BALL_RADIUS;
    double position_y = 0.5 * WINDOW_HEIGHT * (1 + (x/WINDOW_WIDTH)*cos(angle * x));
    vector_t center = {position_x, position_y};
    body_t *real_ball = make_real_ball(center);
    body_t *invisiball = make_invisiball((vector_t){center.x, 0.5 * WINDOW_HEIGHT});
    scene_add_body(state->scene, real_ball);
    scene_add_body(state->scene, invisiball);
  }
  // adds all forces (spring & damping)
  for (size_t i = 1; i < scene_bodies(state->scene); i+=2) {
    create_spring(state->scene, STIFFNESS_K_CONSTANT, scene_get_body(state->scene, i), scene_get_body(state->scene, i+1));
    create_drag(state->scene, DRAG_GAMMA_CONSTANT, scene_get_body(state->scene, i));
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