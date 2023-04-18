#include "state.h"
#include <assert.h>
#include <math.h>

const double SIZE_FACTOR = 1;

const double ENEMY_MASS = 50;
const double ENEMY_SPEED = 40;
const double ENEMY_RELOAD_SPEED = 20;
const size_t ENEMY_COUNT_PYRAMID_TOP = 8;    // 11 - 13 - 15
const size_t ENEMY_COUNT_PYRAMID_BOTTOM = 5; // 6 - 8 - 10
const double ENEMY_HEIGHT = SIZE_FACTOR * 27;
const double ENEMY_WIDTH = ENEMY_HEIGHT * 0.886025404; // length * sqrt(3)/2
const double SPACE_BTWN_ENEMIES = ENEMY_HEIGHT * 0.5;
const double ENEMY_MOVE_FORWARD = 1.2 * ENEMY_HEIGHT; // 0.75*ENEMY_HEIGHT

const double PLAYER_MASS = 500;
const rgb_color_t PLAYER_COLOR = {0, 1, 0};
const size_t DOTS_PER_RADIUS = 10;
const double PLAYER_BODY_RADIUS = SIZE_FACTOR * 20;
const double PLAYER_BODY_ANGLE = 3 * M_PI / 4.0;
const double PLAYER_SPEED = 4 * ENEMY_SPEED;
const vector_t START_POSITION_WINDOW_RATIO = {0.5, 0.05};

const double LASER_MASS = 5;
const rgb_color_t ENEMY_LASER_COLOR = {1, 0, 0};
const rgb_color_t PLAYER_LASER_COLOR = {1, 1, 1};
const vector_t LASER_DIMENSIONS = {SIZE_FACTOR * 2, SIZE_FACTOR * 8};
const double ENEMY_LASER_SPEED = 200;
const double PLAYER_LASER_SPEED = 250;

const double BLOCKADE_MASS = 100;
const size_t BLOCKADE_THICKNESS = 7;
const vector_t BLOCKADE_DIMENSIONS = {SIZE_FACTOR * 4 * PLAYER_BODY_RADIUS,
                                      SIZE_FACTOR *
                                          2}; // width = double player width
const double BLOCKADE_DISTANCE_RATIO = 0.3;   // must be < 0.5
const rgb_color_t BLOCKADE_COLOR = {1, 1, 1};

const rgb_color_t BACKGROUND_COLOR = {0, 0, 0};
const double END_GAME_TIMER = 4;
const size_t WIN_STAR_POINTS = 5;
const double WIN_STAR_INNER_RADIUS = 90;
const double WIN_STAR_OUTER_TO_INNER_RATIO = 2.5;
const rgb_color_t WIN_SCREEN_COLOR = {1, 1, 1};

typedef struct state {
  scene_t *scene;
  body_t *background;
  body_t *player;
  int end_game; // 0 if play, 1 if win, -1 if lose
} state_t;

typedef enum {
  BACKGROUND,
  PLAYER,
  ENEMY,
  PLAYER_LASER,
  ENEMY_LASER,
  BLOCKADE
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

body_t *make_player(double radius, vector_t center, rgb_color_t color) {
  size_t dots = DOTS_PER_RADIUS * radius;
  list_t *shape = list_init(dots + 1, free);
  double angle = PLAYER_BODY_ANGLE / dots;
  vector_t *v = malloc(sizeof(vector_t));
  *v = VEC_ZERO;
  list_add(shape, v);
  for (int i = 0; i < dots; i++) {
    v = malloc(sizeof(vector_t));
    *v = vec_rotate((vector_t){0, radius}, i * angle - 0.5 * PLAYER_BODY_ANGLE);
    list_add(shape, v);
  }
  void *info = info_init(PLAYER, 1);
  body_t *player = body_init_with_info(shape, PLAYER_MASS, color, info, free);
  body_set_centroid(player, center);
  return player;
}

body_t *make_laser(body_type_t type, vector_t center, rgb_color_t color,
                   double velocity_y) {
  list_t *shape = make_rectangle(VEC_ZERO, LASER_DIMENSIONS);
  void *info = info_init(type, 0);
  body_t *laser = body_init_with_info(shape, LASER_MASS, color, info, free);
  body_set_centroid(laser, center);
  body_set_velocity(laser, (vector_t){0, velocity_y});
  return laser;
}

body_t *make_enemy(vector_t center, rgb_color_t color) {
  list_t *shape = make_closed_polygon(0.5 * ENEMY_HEIGHT, 6);
  free(list_remove(shape, 5));
  free(list_remove(shape, 1));
  void *info = info_init(ENEMY, r_double(0, ENEMY_RELOAD_SPEED));
  body_t *enemy = body_init_with_info(shape, ENEMY_MASS, color, info, free);
  body_set_centroid(enemy, center);
  body_set_velocity(enemy, (vector_t){ENEMY_SPEED, 0});
  return enemy;
}

vector_t get_point_0(body_t *body) {
  list_t *shape = body_get_shape(body);
  vector_t point_0 = *(vector_t *)list_get(shape, 0);
  list_free(shape);
  return point_0;
}

vector_t player_get_bottom(body_t *player) { return get_point_0(player); }

vector_t enemy_get_top(body_t *enemy) { return get_point_0(enemy); }

void add_row_of_enemies(scene_t *scene, size_t enemy_count, double pos_y,
                        rgb_color_t color) {
  // asserts row can fit on screen
  assert((ENEMY_HEIGHT + SPACE_BTWN_ENEMIES) * enemy_count < WINDOW_WIDTH);
  double start_x = WINDOW_CENTER.x + 0.5 * ENEMY_WIDTH;
  if (enemy_count % 2 == 0) {
    start_x -= ((enemy_count / 2) * ENEMY_WIDTH +
                ((enemy_count - 1) / 2.0) * SPACE_BTWN_ENEMIES);
  } else {
    start_x -= ((enemy_count / 2) * (ENEMY_WIDTH + SPACE_BTWN_ENEMIES) +
                0.5 * ENEMY_WIDTH);
  }
  for (size_t i = 0; i < enemy_count; i++) {
    vector_t center = {start_x + i * (ENEMY_WIDTH + SPACE_BTWN_ENEMIES), pos_y};
    body_t *enemy = make_enemy(center, color);
    scene_add_body(scene, enemy);
  }
}

void add_blockades(scene_t *scene, double bottom_pos_y) {
  // asserts blockade centers fit on screen
  assert(BLOCKADE_DISTANCE_RATIO < 0.5);
  double pos_y = bottom_pos_y + 0.5 * BLOCKADE_DIMENSIONS.y;
  double dist_btwn_blockade_x = BLOCKADE_DISTANCE_RATIO * WINDOW_WIDTH;
  for (size_t i = 0; i < BLOCKADE_THICKNESS; i++) {
    rgb_color_t color = r_pastel_color();
    for (double pos_x = WINDOW_CENTER.x - dist_btwn_blockade_x;
         pos_x < WINDOW_WIDTH; pos_x += dist_btwn_blockade_x) {
      void *info = info_init(BLOCKADE, 0);
      list_t *shape = make_rectangle(VEC_ZERO, BLOCKADE_DIMENSIONS);
      body_t *blockade =
          body_init_with_info(shape, BLOCKADE_MASS, color, info, free);
      body_set_centroid(blockade, (vector_t){pos_x, pos_y});
      scene_add_body(scene, blockade);
    }
    pos_y += BLOCKADE_DIMENSIONS.y;
  }
}

void enemy_advance(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *enemy = scene_get_body(scene, i);
    if (((info_t *)body_get_info(enemy))->type != ENEMY) {
      continue;
    }
    body_set_centroid(enemy, vec_add(body_get_centroid(enemy),
                                     (vector_t){0, -ENEMY_MOVE_FORWARD}));
    body_set_velocity(enemy, vec_multiply(-1, body_get_velocity(enemy)));
  }
}

int enemy_at_edge(body_t *enemy) {
  double velocity_x = body_get_velocity(enemy).x;
  double center_x = body_get_centroid(enemy).x;
  if (velocity_x > 0 && center_x + 0.5 * ENEMY_WIDTH > WINDOW_WIDTH) {
    return 1;
  } else if (velocity_x < 0 && center_x - 0.5 * ENEMY_WIDTH < 0) {
    return 1;
  }
  return 0;
}

void player_at_edge(body_t *player) {
  double velocity_x = body_get_velocity(player).x;
  double center_x = body_get_centroid(player).x;
  if ((velocity_x > 0 && center_x + PLAYER_BODY_RADIUS >= WINDOW_WIDTH) ||
      (velocity_x < 0 && center_x - PLAYER_BODY_RADIUS <= 0)) {
    body_set_velocity(player, VEC_ZERO);
  }
}

void enemy_shoot(state_t *state, body_t *enemy) {
  info_t *info = (info_t *)body_get_info(enemy);
  if (info->flag > ENEMY_RELOAD_SPEED) {
    vector_t center =
        vec_subtract(enemy_get_top(enemy),
                     (vector_t){0, ENEMY_HEIGHT + 0.5 * LASER_DIMENSIONS.y});
    body_t *laser =
        make_laser(ENEMY_LASER, center, ENEMY_LASER_COLOR, -ENEMY_LASER_SPEED);
    scene_add_body(state->scene, laser);
    // collision btwn laser and player
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, laser);
    list_add(bodies, state->player);
    create_destructive_collision(state->scene, bodies);
    // collision btwn laser and blockades
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *blockade = scene_get_body(state->scene, i);
      if (((info_t *)body_get_info(blockade))->type != BLOCKADE) {
        continue;
      }
      list_t *bodies = list_init(2, NULL);
      list_add(bodies, laser);
      list_add(bodies, blockade);
      create_destructive_collision(state->scene, bodies);
    }
    info->flag = 0;
  }
}

void player_shoot(scene_t *scene, body_t *player) {
  info_t *info = (info_t *)body_get_info(player);
  if (info->flag == 1) {
    vector_t center =
        vec_add(player_get_bottom(player),
                (vector_t){0, PLAYER_BODY_RADIUS + 0.5 * LASER_DIMENSIONS.y});
    body_t *laser = make_laser(PLAYER_LASER, center, PLAYER_LASER_COLOR,
                               PLAYER_LASER_SPEED);
    scene_add_body(scene, laser);
    // collision btwn laser and enemies and btwn laser and blockades
    for (size_t i = 0; i < scene_bodies(scene); i++) {
      body_t *body = scene_get_body(scene, i);
      body_type_t type = ((info_t *)body_get_info(body))->type;
      if (type == ENEMY || type == BLOCKADE) {
        list_t *bodies = list_init(2, NULL);
        list_add(bodies, body);
        list_add(bodies, laser);
        create_destructive_collision(scene, bodies);
      }
    }
    info->flag = 0;
  }
}

void enemy_laser_off_screen(body_t *laser) {
  if (body_get_centroid(laser).y + LASER_DIMENSIONS.y < 0) {
    body_remove(laser);
  }
}

void player_laser_off_screen(body_t *laser) {
  if (body_get_centroid(laser).y - LASER_DIMENSIONS.y > WINDOW_HEIGHT) {
    body_remove(laser);
  }
}

// direction = 1 is right, direction = 2 is left
void player_move(body_t *player, key_event_type_t type, int direction) {
  if (type == KEY_PRESSED) {
    body_set_velocity(player, (vector_t){direction * PLAYER_SPEED, 0});
  } else {
    body_set_velocity(player, VEC_ZERO);
  }
}

void on_key(char key, key_event_type_t type, double held_time, void *state) {
  body_t *player = ((state_t *)state)->player;
  switch (key) {
  case RIGHT_ARROW:
    player_move(player, type, 1);
    break;
  case LEFT_ARROW:
    player_move(player, type, -1);
    break;
  case SPACE_BAR:
    printf("space pressed");
    player_shoot(((state_t *)state)->scene, player);
    break;
  }
}

void win(state_t *state) {
  state->end_game = 1;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t type = ((info_t *)body_get_info(body))->type;
    if (type == PLAYER || type == BACKGROUND) {
      continue;
    }
    body_remove(body);
  }
  body_set_color(state->background, WIN_SCREEN_COLOR);
  body_t *win_star =
      body_init(make_star(WIN_STAR_POINTS, WIN_STAR_INNER_RADIUS,
                          WIN_STAR_OUTER_TO_INNER_RATIO * WIN_STAR_INNER_RADIUS,
                          WINDOW_CENTER, 0),
                0, PLAYER_COLOR);
  scene_add_body(state->scene, win_star);
}

void lose(state_t *state) {
  state->end_game = -1;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t type = ((info_t *)body_get_info(body))->type;
    if (type == ENEMY || type == BACKGROUND) {
      continue;
    }
    body_remove(body);
  }
  body_set_color(state->background, ENEMY_LASER_COLOR);
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->end_game = 0;
  // adds background
  void *bg_info = info_init(BACKGROUND, 0);
  list_t *bg_shape =
      make_rectangle(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  body_t *bg =
      body_init_with_info(bg_shape, 0, BACKGROUND_COLOR, bg_info, free);
  scene_add_body(state->scene, bg);
  state->background = bg;
  // adds player
  vector_t start_position = {WINDOW_WIDTH * START_POSITION_WINDOW_RATIO.x,
                             WINDOW_HEIGHT * START_POSITION_WINDOW_RATIO.y};
  body_t *player =
      make_player(PLAYER_BODY_RADIUS, start_position, PLAYER_COLOR);
  scene_add_body(state->scene, player);
  state->player = player;
  // adds enemies
  double pos_y = WINDOW_HEIGHT - 2 * ENEMY_HEIGHT;
  for (size_t i = ENEMY_COUNT_PYRAMID_TOP; i >= ENEMY_COUNT_PYRAMID_BOTTOM;
       i--) {
    add_row_of_enemies(state->scene, i, pos_y, r_color());
    pos_y -= (ENEMY_HEIGHT + SPACE_BTWN_ENEMIES);
  }
  // adds collisions between player and enemies
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *enemy = scene_get_body(state->scene, i);
    if (((info_t *)body_get_info(enemy))->type != ENEMY) {
      continue;
    }
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, state->player);
    list_add(bodies, enemy);
    create_destructive_collision(state->scene, bodies);
  }
  // adds blockades and collisions between blockades and enemies
  add_blockades(state->scene, start_position.y + PLAYER_BODY_RADIUS);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *blockade = scene_get_body(state->scene, i);
    if (((info_t *)body_get_info(blockade))->type != BLOCKADE) {
      continue;
    }
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *enemy = scene_get_body(state->scene, i);
      if (((info_t *)body_get_info(enemy))->type != ENEMY) {
        continue;
      }
      list_t *bodies = list_init(2, NULL);
      list_add(bodies, blockade);
      list_add(bodies, enemy);
      create_destructive_collision(state->scene, bodies);
    }
  }
  sdl_on_key((void *)on_key);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (!state->end_game) {
    player_at_edge(state->player);
    scene_tick(state->scene, dt);
    int should_advance = 0;
    int player_exists = 0;
    int player_laser_exists = 0;
    size_t enemy_count = 0;
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *body = scene_get_body(state->scene, i);
      info_t *info = (info_t *)body_get_info(body);
      switch (info->type) {
      case (ENEMY):
        if (enemy_get_top(body).y < 0) {
          lose(state);
        }
        enemy_count++;
        info->flag += dt;
        if (!should_advance && enemy_at_edge(body)) {
          should_advance = 1;
        }
        enemy_shoot(state, body);
        break;
      case (ENEMY_LASER):
        enemy_laser_off_screen(body);
        break;
      case (PLAYER_LASER):
        player_laser_off_screen(body);
        player_laser_exists = 1;
        printf("Exists");
        break;
      case (PLAYER):
        player_exists = 1;
        // printf("player's flag: %f\n", ((info_t
        // *)body_get_info(state->player))->flag);
        break;
      case (BLOCKADE):
        break;
      case (BACKGROUND):
        break;
      }
    }
    if (should_advance) {
      enemy_advance(state->scene);
    }
    if (!player_laser_exists) {
      ((info_t *)body_get_info(state->player))->flag = 1;
    }
    if (!player_exists) {
      lose(state);
    }
    if (enemy_count == 0) {
      win(state);
    }
  } else {
    info_t *info = (info_t *)body_get_info(state->background);
    info->flag += dt;
    if (info->flag > END_GAME_TIMER) {
      exit(0);
    }
  }
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}