#include "state.h"
#include <assert.h>
#include <math.h>

const double WALL_ELASTICITY = 1;
const rgb_color_t WALL_COLOR = {1, 1, 1};
const size_t WALL_COUNT = 3;
const double WALL_THICKNESS = 20;

const double BALL_MASS = 50;
const rgb_color_t BALL_COLOR = {1, 1, 0};
const double BALL_RADIUS = 5;
const double DOTS_PER_RADIUS = 10;
const double START_VELOCITY_MAGNITUDE = 350;        // 350
const vector_t START_VELOCITY_X_RANGE = {150, 300}; // {150, 300}

const double BRICK_ELASTICITY = 1.02;
const size_t BRICK_ROW_COUNT = 6; // 6
const size_t BRICKS_PER_ROW = 10; // 12
const double BRICK_HEIGHT = 15;
const double SPACE_BTWN_BRICKS = 10;

const double PADDLE_ELASTICITY = 1;
const rgb_color_t PADDLE_COLOR = {1, 1, 1};
const double PADDLE_SPEED = 500;
const double START_PADDLE_LENGTH = BALL_RADIUS * 10;
const double PADDLE_LENGTH_INCREASE = BALL_RADIUS * 2;
const double PADDLE_HEIGHT = 18;

const double SPACE_BTWN_PADDLE_AND_BOTTOM = 20;
const double SPACE_BTWN_BALL_AND_BOTTOM =
    2 * PADDLE_HEIGHT + SPACE_BTWN_PADDLE_AND_BOTTOM;

const rgb_color_t WIN_SCREEN_COLOR = {0, 0, 0};
const rgb_color_t WIN_STAR_COLOR = {1, 1, 0};
const double END_GAME_TIMER = 4;
const size_t WIN_STAR_POINTS = 5;
const double WIN_STAR_INNER_RADIUS = 90;
const double WIN_STAR_OUTER_TO_INNER_RATIO = 2.5;

const rgb_color_t BACKGROUND_COLOR = {0, 0, 0};

typedef struct state {
  scene_t *scene;
  body_t *background;
  body_t *paddle;
  body_t *ball;
  size_t object_count;
  size_t bricks_destroyed; // used to check when to apply powerups
  int end_game;            // 0 if play, 1 if win, -1 if lose
} state_t;

typedef enum {
  BACKGROUND,
  WALL,
  BRICK,
  PADDLE,
  BALL,
} body_type_t;

typedef struct info {
  body_type_t type;
  double flag; // may be used as timer, boolean, or whatever necessary
} info_t;

void *info_init(body_type_t type, double flag) {
  info_t *info = malloc(sizeof(info_t));
  *info = (info_t){type, flag};
  return (void *)info;
}

void add_background(state_t *state) {
  void *bg_info = info_init(BACKGROUND, 0);
  list_t *bg_shape =
      make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  body_t *bg =
      body_init_with_info(bg_shape, 0, BACKGROUND_COLOR, bg_info, free);
  scene_add_body(state->scene, bg);
  state->background = bg;
}

void add_walls(scene_t *scene) {
  void *wall_info = info_init(WALL, 0);
  body_t *right = body_init_with_info(
      make_rectangle((vector_t){WINDOW_WIDTH, 0},
                     (vector_t){WINDOW_WIDTH + WALL_THICKNESS, WINDOW_HEIGHT}),
      INFINITY, WALL_COLOR, wall_info, free);
  body_set_removability(right, false);
  scene_add_body(scene, right);

  wall_info = info_init(WALL, 0);
  body_t *top = body_init_with_info(
      make_rectangle((vector_t){0, WINDOW_HEIGHT},
                     (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT + WALL_THICKNESS}),
      INFINITY, WALL_COLOR, wall_info, free);
  body_set_removability(top, false);
  scene_add_body(scene, top);

  wall_info = info_init(WALL, 0);
  body_t *left = body_init_with_info(
      make_rectangle(VEC_ZERO, (vector_t){-WALL_THICKNESS, WINDOW_HEIGHT}),
      INFINITY, WALL_COLOR, wall_info, free);
  body_set_removability(left, false);
  scene_add_body(scene, left);
}

void add_paddle_and_ball(state_t *state) {
  void *paddle_info = info_init(PADDLE, 0);
  list_t *paddle_shape =
      make_rectangle(VEC_ZERO, (vector_t){BALL_RADIUS, PADDLE_HEIGHT});
  body_t *paddle = body_init_with_info(paddle_shape, INFINITY, PADDLE_COLOR,
                                       paddle_info, free);
  body_set_removability(paddle, false);
  scene_add_body(state->scene, paddle);
  state->paddle = paddle;

  void *ball_info = info_init(BALL, 0);
  list_t *ball_shape = make_circle(BALL_RADIUS, DOTS_PER_RADIUS, VEC_ZERO);
  body_t *ball =
      body_init_with_info(ball_shape, BALL_MASS, BALL_COLOR, ball_info, free);
  body_set_removability(ball, false);
  scene_add_body(state->scene, ball);
  state->ball = ball;
}

void add_row_of_bricks(scene_t *scene, double pos_y, rgb_color_t color) {
  double brick_width =
      (WINDOW_WIDTH / (1.0 * (BRICKS_PER_ROW)) - SPACE_BTWN_BRICKS);
  for (size_t i = 0; i < BRICKS_PER_ROW; i++) {
    void *brick_info = info_init(BRICK, 0);
    vector_t center = {(SPACE_BTWN_BRICKS + brick_width) / 2.0 +
                           (i * (brick_width + SPACE_BTWN_BRICKS)),
                       pos_y};
    list_t *shape =
        make_rectangle(VEC_ZERO, (vector_t){brick_width, BRICK_HEIGHT});
    body_t *brick =
        body_init_with_info(shape, INFINITY, color, brick_info, free);
    body_set_centroid(brick, center);
    scene_add_body(scene, brick);
  }
}

void add_bricks(state_t *state) {
  double pos_y = WINDOW_HEIGHT - 0.5 * (SPACE_BTWN_BRICKS + BRICK_HEIGHT);
  for (size_t i = 0; i < BRICK_ROW_COUNT; i++) {
    add_row_of_bricks(state->scene, pos_y, r_pastel_color());
    pos_y -= (SPACE_BTWN_BRICKS + BRICK_HEIGHT);
  }
  state->bricks_destroyed = 0;
  state->object_count = 0;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    if (!body_is_removed(scene_get_body(state->scene, i))) {
      state->object_count++;
    }
  }
}

void make_ball_wall_paddle_bounce(state_t *state) {
  // Ball-Paddle Physics Collision
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, state->ball);
  list_add(bodies, state->paddle);
  create_physics_collision(state->scene, PADDLE_ELASTICITY, bodies);

  // Ball-Walls Physics Collision
  size_t walls_counted = 0;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *wall = scene_get_body(state->scene, i);
    info_t *info = (info_t *)body_get_info(wall);
    if (info->type == WALL) {
      bodies = list_init(2, NULL);
      list_add(bodies, state->ball);
      list_add(bodies, wall);
      create_physics_collision(state->scene, WALL_ELASTICITY, bodies);
      walls_counted++;
    }
    if (walls_counted == WALL_COUNT) {
      break;
    }
  }
}

void make_ball_brick_bounce_and_break(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *brick = scene_get_body(state->scene, i);
    info_t *info = (info_t *)body_get_info(brick);
    if (info->type == BRICK) {
      // destructive collision
      list_t *bodies = list_init(2, NULL);
      list_add(bodies, state->ball);
      list_add(bodies, brick);
      create_destructive_collision(state->scene, bodies);
      // physics collision
      bodies = list_init(2, NULL);
      list_add(bodies, state->ball);
      list_add(bodies, brick);
      create_physics_collision(state->scene, BRICK_ELASTICITY, bodies);
    }
  }
}

double get_paddle_length(body_t *paddle) {
  list_t *paddle_shape = body_get_shape(paddle);
  double length = vec_subtract(*(vector_t *)list_get(paddle_shape, 0),
                               *(vector_t *)list_get(paddle_shape, 2))
                      .x;
  list_free(paddle_shape);
  return length;
}

void center_paddle_position(body_t *paddle) {
  body_set_centroid(
      paddle, (vector_t){0.5 * WINDOW_WIDTH, body_get_centroid(paddle).y});
}

void inverse_paddle_position(body_t *paddle) {
  vector_t center = body_get_centroid(paddle);
  body_set_centroid(paddle, (vector_t){WINDOW_WIDTH - center.x, center.y});
}

void increase_paddle_length(body_t *paddle, double size_increase) {
  double size_factor = 1.0 + size_increase / get_paddle_length(paddle);
  body_dilate_x(paddle, size_factor);
}

void reset_paddle_length(body_t *paddle) {
  double resize_factor = (BALL_RADIUS * 1.0) / get_paddle_length(paddle);
  body_dilate_x(paddle, resize_factor);
}

void reset_ball_and_paddle(state_t *state) {
  body_set_centroid(state->paddle, (vector_t){0.5 * WINDOW_WIDTH,
                                              SPACE_BTWN_PADDLE_AND_BOTTOM});
  body_set_velocity(state->paddle, VEC_ZERO);
  body_set_centroid(state->ball,
                    (vector_t){0.5 * WINDOW_WIDTH, SPACE_BTWN_BALL_AND_BOTTOM});
  double velocity_x =
      r_sign() * r_double(START_VELOCITY_X_RANGE.x, START_VELOCITY_X_RANGE.y);
  double velocity_y = sqrt(START_VELOCITY_MAGNITUDE * START_VELOCITY_MAGNITUDE -
                           velocity_x * velocity_x);
  body_set_velocity(state->ball, (vector_t){velocity_x, velocity_y});
}

void reset(state_t *state) {
  // removes all bricks
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    info_t *info = (info_t *)body_get_info(body);
    if (info->type == BRICK) {
      body_remove(body);
    }
  }
  add_bricks(state);
  make_ball_brick_bounce_and_break(state);
  reset_paddle_length(state->paddle);
  reset_ball_and_paddle(state);
}

void paddle_at_edge(state_t *state) {
  double paddle_length = get_paddle_length(state->paddle);
  double velocity_x = body_get_velocity(state->paddle).x;
  double center_x = body_get_centroid(state->paddle).x;
  if ((velocity_x > 0 && center_x + 0.5 * paddle_length >= WINDOW_WIDTH) ||
      (velocity_x < 0 && center_x - 0.5 * paddle_length <= 0)) {
    body_set_velocity(state->paddle, VEC_ZERO);
  }
}

void check_loss(state_t *state) {
  if (body_get_centroid(state->ball).y - BALL_RADIUS < 0) {
    reset(state);
  }
}

// direction = 1 is right, direction = 2 is left
void paddle_move(body_t *paddle, key_event_type_t type, int direction) {
  if (type == KEY_PRESSED) {
    body_set_velocity(paddle, (vector_t){direction * PADDLE_SPEED, 0});
  } else {
    body_set_velocity(paddle, VEC_ZERO);
  }
}

void on_key(char key, key_event_type_t type, double held_time, void *state) {
  state_t *state_casted = (state_t *)state;
  if (type == KEY_PRESSED) {
    switch (key) {
    case Z_KEY:
      center_paddle_position(state_casted->paddle);
      break;
    case X_KEY:
      inverse_paddle_position(state_casted->paddle);
      break;
    case R_KEY:
      reset(state_casted);
      break;
    case Q_KEY:
      printf("EXITED");
      abort();
    }
  }
  switch (key) {
  case RIGHT_ARROW:
    paddle_move(state_casted->paddle, type, 1);
    break;
  case LEFT_ARROW:
    paddle_move(state_casted->paddle, type, -1);
    break;
  }
}

void win(state_t *state) {
  state->end_game = 1;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t type = ((info_t *)body_get_info(body))->type;
    if (type == PADDLE || type == BACKGROUND) {
      continue;
    }
    body_remove(body);
  }
  body_set_color(state->background, WIN_SCREEN_COLOR);
  body_t *win_star =
      body_init(make_star(WIN_STAR_POINTS, WIN_STAR_INNER_RADIUS,
                          WIN_STAR_OUTER_TO_INNER_RATIO * WIN_STAR_INNER_RADIUS,
                          WINDOW_CENTER, 0),
                0, WIN_STAR_COLOR);
  scene_add_body(state->scene, win_star);
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->end_game = 0;
  add_background(state);
  add_walls(state->scene);
  add_paddle_and_ball(state);
  add_bricks(state);
  body_set_centroid(state->ball, (vector_t){WINDOW_CENTER.x, WINDOW_CENTER.y});
  reset_ball_and_paddle(state);
  make_ball_wall_paddle_bounce(state);
  make_ball_brick_bounce_and_break(state);
  sdl_on_key((void *)on_key);

  // Game Instructions
  printf("BREAKOUT!!!\n");
  printf("Break all the bricks and you win / Let the ball hit the bottom and "
         "you lose\n");
  printf("Every brick you break the ball gets faster, but every 5 bricks the "
         "paddle gets longer\n");
  printf("Use the Arrow Keys to move the paddle across the screen\n");
  printf("Press Z to center the paddle / Press X to reflect the paddle to the "
         "opposite side\n");
  printf("Press Q to quit and R to restart\n");
  printf("Enjoy!\n");
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (!state->end_game) {
    if (get_paddle_length(state->paddle) < START_PADDLE_LENGTH) {
      increase_paddle_length(state->paddle, 0.5 * BALL_RADIUS);
    }
    paddle_at_edge(state);
    scene_tick(state->scene, dt);
    check_loss(state);
    size_t brick_count = 0;
    if (state->object_count > scene_bodies(state->scene)) {
      state->bricks_destroyed +=
          (state->object_count - scene_bodies(state->scene));
      state->object_count = scene_bodies(state->scene);
    }
    if (state->bricks_destroyed >= 5) {
      increase_paddle_length(state->paddle, PADDLE_LENGTH_INCREASE);
      state->bricks_destroyed -= 5;
    }
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *body = scene_get_body(state->scene, i);
      info_t *info = (info_t *)body_get_info(body);
      if (info->type == BRICK) {
        brick_count++;
      }
    }
    if (brick_count == 0) {
      win(state);
    }
  }
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}