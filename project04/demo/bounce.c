#include "scene.h"
#include "state.h"

const size_t STAR_NUM = 5;
const size_t STAR_POINTS = 5;
const vector_t SPEED_BOUNDS = {75, 550};
const vector_t ROTATION_BOUNDS = {1, 10};
const vector_t STAR_SIZE_RANGE = (vector_t){15, 30};

typedef struct state {
  scene_t *scene;
} state_t;

body_t *new_body_behavior(body_t *body) {
  body_set_color(body, r_color());
  body_set_angular_velocity(body, r_double(ROTATION_BOUNDS.x, ROTATION_BOUNDS.y) *
                                 r_sign());
  vector_t v = body_get_velocity(body);
  
  // Changes speed without changing direction
  v = vec_multiply(1 / fabs(fabs(v.x) > fabs(v.y) ? v.x : v.y), v);
  v = vec_multiply(r_double(SPEED_BOUNDS.x, SPEED_BOUNDS.y), v);
  body_set_velocity(body, v);
  return body;
}

state_t *emscripten_init() {
  srand(time(NULL));
  state_t *cur_state = malloc(sizeof(state_t));
  cur_state->scene = scene_init();

  for (size_t i = 0; i < STAR_NUM; i++) {
    double radius = r_int(STAR_SIZE_RANGE.x, STAR_SIZE_RANGE.y);
    list_t *star = make_star(STAR_POINTS, radius / 2, radius, WINDOW_CENTER, 0);
    body_t *body = body_init(star, 5, r_color());
    vector_t speed;
    speed.x = r_double(SPEED_BOUNDS.x, SPEED_BOUNDS.y) * r_sign();
    speed.y = r_double(SPEED_BOUNDS.x, SPEED_BOUNDS.y) * r_sign();
    body_set_velocity(body, speed);
    new_body_behavior(body);
    scene_add_body(cur_state->scene, body);
  }
  sdl_init(VEC_ZERO, vec_multiply(2, WINDOW_CENTER));
  return cur_state;
}

void check_collision(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    list_t *shape = body_get_shape(body);
    for (size_t j = 0; j < list_size(shape); j++) {
      vector_t *vec = list_get(shape, j);
      // check x
      vector_t vel = body_get_velocity(body);
      if ((vec->x >= WINDOW_WIDTH && vel.x > 0) ||
          (vec->x <= VEC_ZERO.x && vel.x < 0)) {
        vel.x *= -1;
        body_set_velocity(body, vel);
        new_body_behavior(body);
        break;
      }
      // check y
      if ((vec->y >= WINDOW_HEIGHT && vel.y > 0) ||
          (vec->y <= VEC_ZERO.y && vel.y < 0)) {
        vel.y *= -1;
        body_set_velocity(body, vel);
        new_body_behavior(body);
        break;
      }
    }
    list_free(shape);
  }
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  check_collision(state);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}