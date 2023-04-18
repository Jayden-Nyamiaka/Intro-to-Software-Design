#include "state.h"
#include <assert.h>
#include <math.h>

// ALL DISTANCE MEASUREMENTS ARE IN PROPORTION TO WINDOW WIDTH AND HEIGHT
const size_t SCREEN_COUNT = 6;
const double LITTLE_G_CONSTANT = 2.5;

const rgb_color_t WALL_COLOR = {1, 1, 1};
const double WALL_THICKNESS = 50 / 2400.0;
const double FLOOR_HEIGHT = 150 / 1800.0;

const double QUEEN_MASS = 100;
const rgb_color_t QUEEN_COLOR = {1, 0.412, 0.706};
const vector_t QUEEN_DIM = {100 / 2400.0, 125 / 1800.0};
const double CROUCH_HEIGHT = 70 / 1800.0;
const double QUEEN_ELASTICITY = 0.5;
const double JUMP_Y_VELOCITY = 2.41; // 2.41
const double JUMP_X_SPEED = 0.42;
const double MAX_JUMP_CHARGE = 0.6;
const double QUEEN_SPEED = 0.18;
const double QUEEN_TERMINAL_VELOCITY = 1.7;

const rgb_color_t STATIONARY_PLATFORM_COLOR = {197 / 255.0, 199 / 255.0,
                                               196 / 255.0};
const rgb_color_t MOVING_PLATFORM_COLOR = {1, 1, 220 / 255.0};
const rgb_color_t DISAPPEARING_PLATFORM_COLOR = {197 / 255.0, 180 / 255.0,
                                                 227 / 255.0};
const rgb_color_t SLIPPERY_PLATFORM_COLOR = {186 / 255.0, 219 / 255.0, 1};
const vector_t MOVING_PLATFORM_VELOCITY = {0.1, 0.1};
const double APPERANCE_INTERVAL = 4.0; // in seconds
const double JUST_APPEARED_BOUND = 0.05;
const double SLIP_ACCELERATION_MAGNITUDE = 0.5;
const double SLIP_FRICTION_CONSTANT = 0.5;
const double TERMINAL_SLIP_SPEED = 0.3;

const rgb_color_t WIN_SCREEN_COLOR = {0, 0, 0};
const rgb_color_t WIN_STAR_COLOR = {1, 1, 0};
const double END_GAME_TIMER = 4;
const size_t WIN_STAR_POINTS = 5;
const double WIN_STAR_INNER_RADIUS = 90;
const double WIN_STAR_OUTER_TO_INNER_RATIO = 2.5;

const rgb_color_t BACKGROUND_COLOR = {0, 0, 0};

typedef enum { LEFT, RIGHT, BOTTOM, TOP } direction_t;

typedef enum { START = -1, PLAY = 0, WIN = 1, PAUSE = 2 } status_t;

typedef struct state {
  status_t status;
  status_t last_status;
  int screen_num;
  scene_t *scene;
  body_t *background;
  body_t *queen;
} state_t;

typedef enum {
  BACKGROUND,
  QUEEN,
  STATIONARY_PLATFORM,
  MOVING_X_PLATFORM,
  MOVING_Y_PLATFORM,
  SLIPPERY_PLATFORM, // ice or rain
  DISAPPEARING_PLATFORM
} body_type_t;

typedef struct basic_info {
  body_type_t type;
  void *traits;
} info_t;

typedef struct platform_traits {
  vector_t bounds; // used for moving platform
  double interval; // used for disappearing platform
} platform_traits_t;

typedef struct queen_traits {
  int last_direction_pressed;
  double jump_charge_timer;
  bool charging;
  bool jump_released;
  double face_plant_timer;

  body_t *movement_medium;

  bool in_air;
  bool touching_bottom;

  bool can_move_left;
  bool can_move_right;

  int counter;
} queen_traits_t;

info_t *info_init(body_type_t type, void *traits) {
  info_t *info = malloc(sizeof(info_t));
  info->type = type;
  info->traits = traits;
  return info;
}

void free_info(info_t *info) {
  free(info->traits);
  free(info);
}

info_t *platform_info_init(body_type_t type) {
  platform_traits_t *traits = malloc(sizeof(platform_traits_t));
  *traits = (platform_traits_t){VEC_ZERO, 0.0};
  return info_init(type, traits);
}

info_t *moving_platform_info_init(body_type_t type, vector_t bounds) {
  info_t *info = platform_info_init(type);
  ((platform_traits_t *)(info->traits))->bounds = bounds;
  return info;
}

info_t *queen_info_init() {
  queen_traits_t *traits = malloc(sizeof(queen_traits_t));
  traits->last_direction_pressed = 0;
  traits->jump_charge_timer = 0.0;
  traits->charging = false;
  traits->jump_released = true;
  traits->face_plant_timer = 0;

  traits->movement_medium = NULL;

  traits->in_air = false;
  traits->touching_bottom = false;

  traits->can_move_left = true;
  traits->can_move_right = true;

  traits->counter = 0;
  return info_init(QUEEN, traits);
}

void queen_zero_flags(body_t *queen) {
  queen_traits_t *traits = ((info_t *)body_get_info(queen))->traits;
  traits->touching_bottom = false;
  traits->can_move_left = true;
  traits->can_move_right = true;
}

void add_background(state_t *state) {
  list_t *bg_shape = make_rectangle(
      VEC_ZERO, (vector_t){WINDOW_WIDTH, SCREEN_COUNT * WINDOW_HEIGHT});
  state->background = body_init_with_info(bg_shape, 0, BACKGROUND_COLOR,
                                          info_init(BACKGROUND, NULL), free);
  scene_add_body(state->scene, state->background);
}

double get_coordinate(body_t *body, direction_t side) {
  list_t *shape = body_get_shape(body);
  double out;
  switch (side) {
  case (LEFT):
    out = ((vector_t *)list_get(shape, 2))->x;
    break;
  case (RIGHT):
    out = ((vector_t *)list_get(shape, 0))->x;
    break;
  case (BOTTOM):
    out = ((vector_t *)list_get(shape, 2))->y;
    break;
  case (TOP):
    out = ((vector_t *)list_get(shape, 0))->y;
    break;
  }
  list_free(shape);
  return out;
}

void add_queen_hitbox(state_t *state) {
  body_t *lowest_platform = NULL;
  double lowest_coordinate;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *platform = scene_get_body(state->scene, i);
    info_t *info = body_get_info(platform);
    if (info->type == BACKGROUND || info->type == QUEEN) {
      continue;
    }
    double coordinate = get_coordinate(platform, TOP);
    if (lowest_platform == NULL) {
      lowest_platform = platform;
      lowest_coordinate = coordinate;
      continue;
    }
    if (coordinate < lowest_coordinate) {
      lowest_platform = platform;
      lowest_coordinate = coordinate;
    }
  }

  vector_t bottom_left = {body_get_centroid(lowest_platform).x -
                              0.5 * QUEEN_DIM.x * WINDOW_WIDTH,
                          lowest_coordinate + 2};
  vector_t top_right = vec_add(bottom_left, vec_scale(QUEEN_DIM, WINDOW_DIM));
  info_t *info = queen_info_init();
  state->queen =
      body_init_with_info(make_rectangle(bottom_left, top_right), QUEEN_MASS,
                          QUEEN_COLOR, info, (free_func_t)free_info);
  scene_add_body(state->scene, state->queen);
}

void queen_toggle_crouch(body_t *queen, bool crouch) {
  double crouch_distance = (QUEEN_DIM.y - CROUCH_HEIGHT) * WINDOW_HEIGHT;
  if (crouch) {
    crouch_distance *= -1;
  }
  list_t *queen_shape = body_get_real_shape(queen);
  for (size_t i = 0; i < 2; i++) {
    vector_t *point = (vector_t *)list_get(queen_shape, i);
    *point = (vector_t){point->x, point->y + crouch_distance};
  }
}

// returns 0 if false, -1 if outside lower bound, 1 if outside upper bound
int platform_outside_boundaries(body_t *platform) {
  info_t *info = (info_t *)body_get_info(platform);
  vector_t bounds = ((platform_traits_t *)info->traits)->bounds;
  // x bounds
  if (info->type == MOVING_X_PLATFORM) {
    if (get_coordinate(platform, LEFT) < bounds.x) {
      return -1;
    } else if (get_coordinate(platform, RIGHT) > bounds.y) {
      return 1;
    } else {
      return 0;
    }
  }
  // y bounds
  if (get_coordinate(platform, BOTTOM) < bounds.x) {
    return -1;
  } else if (get_coordinate(platform, TOP) > bounds.y) {
    return 1;
  } else {
    return 0;
  }
}

// returns newly made platform
body_t *make_platform(body_type_t type, rgb_color_t color,
                      vector_t bottom_left_corner, vector_t top_right_corner) {
  assert(bottom_left_corner.x < top_right_corner.x &&
         bottom_left_corner.y < top_right_corner.y);
  info_t *info = platform_info_init(type);
  body_t *platform =
      body_init_with_info(make_rectangle(bottom_left_corner, top_right_corner),
                          INFINITY, color, info, (free_func_t)free_info);
  body_set_removability(platform, false);
  return platform;
}

// ADD PLATFORMS TO SCENE AND RETURNS THEM

body_t *add_stationary_platform(scene_t *scene, vector_t bottom_left_corner,
                                vector_t top_right_corner) {
  body_t *stationary_platform =
      make_platform(STATIONARY_PLATFORM, STATIONARY_PLATFORM_COLOR,
                    bottom_left_corner, top_right_corner);
  scene_add_body(scene, stationary_platform);
  return stationary_platform;
}

// direction == 'x': MOVING_X_PLATFORM / direction == 'y': MOVING_Y_PLATFORM
body_t *add_moving_platform(scene_t *scene, vector_t bottom_left_corner,
                            vector_t top_right_corner, double velocity,
                            vector_t bounds, char direction) {
  body_type_t type;
  vector_t vel;
  if (direction == 'y') {
    vel = (vector_t){0, velocity};
    type = MOVING_Y_PLATFORM;
  } else {
    vel = (vector_t){velocity, 0};
    type = MOVING_X_PLATFORM;
  }
  body_t *moving_platform = make_platform(type, MOVING_PLATFORM_COLOR,
                                          bottom_left_corner, top_right_corner);
  platform_traits_t *traits =
      (platform_traits_t *)((info_t *)body_get_info(moving_platform))->traits;
  traits->bounds = bounds;
  assert(!platform_outside_boundaries(moving_platform));
  body_set_velocity(moving_platform, vel);
  scene_add_body(scene, moving_platform);
  return moving_platform;
}

body_t *add_disappearing_platform(scene_t *scene, vector_t bottom_left_corner,
                                  vector_t top_right_corner) {
  body_t *disappearing_platform =
      make_platform(DISAPPEARING_PLATFORM, DISAPPEARING_PLATFORM_COLOR,
                    bottom_left_corner, top_right_corner);
  scene_add_body(scene, disappearing_platform);
  return disappearing_platform;
}

body_t *add_slippery_platform(scene_t *scene, vector_t bottom_left_corner,
                              vector_t top_right_corner) {
  body_t *slippery_platform =
      make_platform(SLIPPERY_PLATFORM, SLIPPERY_PLATFORM_COLOR,
                    bottom_left_corner, top_right_corner);
  scene_add_body(scene, slippery_platform);
  return slippery_platform;
}

void control_special_platform_behavior(state_t *state, double dt) {
  queen_traits_t *queen_traits =
      (queen_traits_t *)((info_t *)body_get_info(state->queen))->traits;
  // applies slip friction (only when on slippery platform)
  if (queen_traits->in_air && body_get_velocity(state->queen).x != 0 &&
      ((info_t *)body_get_info(queen_traits->movement_medium))->type ==
          SLIPPERY_PLATFORM) {
    double friction_force_x = SLIP_FRICTION_CONSTANT *
                              body_get_mass(state->queen) * LITTLE_G_CONSTANT;
    if (body_get_velocity(state->queen).x > 0) {
      friction_force_x *= -1;
    }
    body_add_force(state->queen, (vector_t){friction_force_x, 0});
  }
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *platform = scene_get_body(state->scene, i);
    info_t *info = body_get_info(platform);
    platform_traits_t *platform_traits = (platform_traits_t *)info->traits;
    if (info->type == MOVING_X_PLATFORM || info->type == MOVING_Y_PLATFORM) {
      int direction = -1 * platform_outside_boundaries(platform);
      if (direction) {
        vector_t platform_velocity = body_get_velocity(platform);
        if (!queen_traits->in_air && queen_traits->movement_medium == platform) {
          body_set_velocity(state->queen,
                            vec_subtract(body_get_velocity(state->queen),
                                         vec_multiply(2.0, platform_velocity)));
        }
        vector_t unsigned_velocity = {fabs(platform_velocity.x),
                                      fabs(platform_velocity.y)};
        body_set_velocity(platform, vec_multiply(direction, unsigned_velocity));
      }
    } else if (info->type == DISAPPEARING_PLATFORM) {
      platform_traits->interval += dt;
      if (platform_traits->interval > APPERANCE_INTERVAL) {
        double translation = WINDOW_WIDTH;
        if (body_get_centroid(platform).x > WINDOW_WIDTH) {
          translation *= -1;
        }
        body_translate(platform, (vector_t){translation, 0});
        platform_traits->interval = 0;
      }
    }
  }
}

void conditional_gravity(void *aux) {
  state_t *state = (state_t *)aux;
  queen_traits_t *traits =
      (queen_traits_t *)((info_t *)body_get_info(state->queen))->traits;
  traits->in_air = !traits->touching_bottom;
  // uncrouches once queen start jumping
  if (traits->in_air && traits->charging) {
    traits->charging = false;
    queen_toggle_crouch(state->queen, false);
  }
  if (!traits->in_air) {
    return;
  }
  traits->can_move_left = false;
  traits->can_move_right = false;
  vector_t velocity = body_get_velocity(state->queen);
  if (velocity.y == -QUEEN_TERMINAL_VELOCITY * WINDOW_HEIGHT) {
    return;
  }
  if (velocity.y < -QUEEN_TERMINAL_VELOCITY * WINDOW_HEIGHT) {
    body_set_velocity(
        state->queen,
        (vector_t){velocity.x, -QUEEN_TERMINAL_VELOCITY * WINDOW_HEIGHT});
    return;
  }
  vector_t force = {0, -1 * body_get_mass(state->queen) * LITTLE_G_CONSTANT *
                           WINDOW_HEIGHT};
  body_add_force(state->queen, force);
}

void adjust_to_platform(body_t *queen, body_t *platform,
                        direction_t queen_side) {
  // adjust so that use does not see adjustment
  vector_t difference;
  switch (queen_side) {
  case (BOTTOM):
    difference = (vector_t){0, get_coordinate(platform, TOP) -
                                   get_coordinate(queen, BOTTOM)};
    break;
  case (TOP):
    difference = (vector_t){0, get_coordinate(platform, BOTTOM) -
                                   get_coordinate(queen, TOP)};
    break;
  case (RIGHT):
    difference = (vector_t){
        get_coordinate(platform, LEFT) - get_coordinate(queen, RIGHT), 0};
    break;
  case (LEFT):
    difference = (vector_t){
        get_coordinate(platform, RIGHT) - get_coordinate(queen, LEFT), 0};
    break;
  default:
    difference = VEC_ZERO;
  }
  if (difference.x == 0 && difference.y == 0) {
    return;
  }
  body_translate(queen, difference);
}

/** Custom collision handler for stationary platform collisions */
void platform_collide(bool last_tick_collision, body_t *queen, body_t *platform,
                      vector_t axis, void *aux) {
  queen_traits_t *queen_traits =
      (queen_traits_t *)((info_t *)body_get_info(queen))->traits;
  info_t *platform_info = (info_t *)body_get_info(platform);
  platform_traits_t *platform_traits =
      (platform_traits_t *)platform_info->traits;
  if (platform_traits->interval > 0 &&
      platform_traits->interval < JUST_APPEARED_BOUND) {
    adjust_to_platform(queen, platform, TOP);
    body_set_velocity(queen, (vector_t){0, -QUEEN_TERMINAL_VELOCITY});
    // make disapperance collision sound effect
    return;
  }
  if (!last_tick_collision) {
    printf("%d: (%f, %f):\n", queen_traits->counter, axis.x, axis.y);
    queen_traits->counter++;

    // remember to delete counter
  }
  vector_t velocity = body_get_velocity(queen);
  vector_t queen_centroid = body_get_centroid(queen);
  vector_t platform_centroid = body_get_centroid(platform);
  if (axis.y == 1 || axis.y == -1) {
    // queen's bottom collision
    if (queen_centroid.y > platform_centroid.y) {
      queen_traits->touching_bottom = true;
      queen_traits->in_air = false;

      if (!last_tick_collision) {
        printf("BOTTOM, in_air = %d\n", queen_traits->in_air);
      }

      adjust_to_platform(queen, platform, BOTTOM);
      queen_traits->movement_medium = platform;
      // body_reset_force_and_impulse(queen);
      if (!last_tick_collision) {
        // commence face plant
        if (velocity.y <= -QUEEN_TERMINAL_VELOCITY * WINDOW_HEIGHT) {
          queen_traits->face_plant_timer = 0.00001;
        }
        if (platform_info->type != SLIPPERY_PLATFORM) {
          body_set_velocity(queen,
                            body_get_velocity(queen_traits->movement_medium));
        } else {
          body_set_velocity(queen, (vector_t){body_get_velocity(queen).x, 0});
        }
      }
      return;
      // queen's top collision
    } else {
      // computes physics collision for top
      if (!last_tick_collision) {
        printf("TOP, in_air = %d\n", queen_traits->in_air);
      }

      if (queen_traits->in_air) {
        handle_physics_collision(last_tick_collision, queen, platform, axis,
                                 aux);
      }
      return;
    }
  }
  if (axis.x == 1 || axis.x == -1) {
    // queen's left collision
    if (queen_centroid.x > platform_centroid.x) {
      if (!last_tick_collision) {
        printf("LEFT, in_air = %d\n", queen_traits->in_air);
      }

      queen_traits->can_move_left = false;
      if (!queen_traits->in_air && velocity.x < 0) {
        body_set_velocity(queen, VEC_ZERO);
        adjust_to_platform(queen, platform, LEFT);
      }
      // queen's right collision
    } else {
      if (!last_tick_collision) {
        printf("RIGHT, in_air = %d\n", queen_traits->in_air);
      }

      queen_traits->can_move_right = false;
      if (!queen_traits->in_air && velocity.x > 0) {
        body_set_velocity(queen, VEC_ZERO);
        adjust_to_platform(queen, platform, RIGHT);
      }
    }
    // computes physics collision for both right and left
    if (queen_traits->in_air) {
      handle_physics_collision(last_tick_collision, queen, platform, axis, aux);
    }
  }
}

void make_platform_collisions(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *platform = scene_get_body(state->scene, i);
    info_t *info = (info_t *)body_get_info(platform);
    if (info->type == BACKGROUND || info->type == QUEEN) {
      continue;
    }
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, state->queen);
    list_add(bodies, platform);
    void *aux = physics_collision_aux_init(QUEEN_ELASTICITY);
    create_collision(state->scene, bodies, platform_collide, aux, free);
  }
}

void add_walls(scene_t *scene) {
  // left wall
  add_stationary_platform(
      scene, VEC_ZERO,
      vec_scale((vector_t){WALL_THICKNESS, SCREEN_COUNT}, WINDOW_DIM));
  // right wall
  add_stationary_platform(
      scene, vec_scale((vector_t){1.0 - WALL_THICKNESS, 0}, WINDOW_DIM),
      vec_scale((vector_t){1.0, SCREEN_COUNT}, WINDOW_DIM));
}

void raise_screen_of_platforms(list_t *screen, size_t screen_number) {
  if (screen_number == 0) {
    return;
  }
  vector_t translation = (vector_t){0, screen_number * WINDOW_HEIGHT};
  for (size_t i = 0; i < list_size(screen); i++) {
    body_t *platform = (body_t *)list_get(screen, i);
    info_t *info = (info_t *)body_get_info(platform);
    body_translate(platform, translation);
    if (info->type == MOVING_Y_PLATFORM) {
      platform_traits_t *traits = (platform_traits_t *)info->traits;
      vector_t new_bounds = {traits->bounds.x + translation.y,
                             traits->bounds.y + translation.y};
      traits->bounds = new_bounds;
    }
  }
  list_free(screen);
}

void add_screen_1(scene_t *scene) {
  // floor [0]
  add_stationary_platform(
      scene, VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT * FLOOR_HEIGHT});
  // right bottom platform [1]
  add_stationary_platform(
      scene, vec_scale((vector_t){WALL_THICKNESS, FLOOR_HEIGHT}, WINDOW_DIM),
      vec_scale((vector_t){650 / 2400.0, 875 / 1800.0}, WINDOW_DIM));
  // left bottom platform [2]
  add_stationary_platform(
      scene, vec_scale((vector_t){1750 / 2400.0, FLOOR_HEIGHT}, WINDOW_DIM),
      vec_scale((vector_t){1.0 - WALL_THICKNESS, 875 / 1800.0}, WINDOW_DIM));
  // top mid platform [3]
  add_stationary_platform(
      scene, vec_scale((vector_t){925 / 2400.0, 1350 / 1800.0}, WINDOW_DIM),
      vec_scale((vector_t){1475 / 2400.0, 1600 / 1800.0}, WINDOW_DIM));
}

void add_screen_2(scene_t *scene) {
  list_t *screen = list_init(5, NULL);
  // most bottom platform [0]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){1600 / 2400.0, 160 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1960 / 2400.0, 320 / 1800.0}, WINDOW_DIM)));
  // most right platform [1]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){2040 / 2400.0, 640 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1.0 - WALL_THICKNESS, 810 / 1800.0},
                         WINDOW_DIM)));
  // bottom right middle platform [2]
  list_add(
      screen,
      add_moving_platform(
          scene, vec_scale((vector_t){700 / 2400.0, 640 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1060 / 2400.0, 810 / 1800.0}, WINDOW_DIM),
          MOVING_PLATFORM_VELOCITY.x * WINDOW_WIDTH,
          vec_multiply(WINDOW_WIDTH, (vector_t){530 / 2400.0, 1550 / 2400.0}),
          'x'));
  // top left middle platform [3]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){600 / 2400.0, 1050 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){900 / 2400.0, 1290 / 1800.0}, WINDOW_DIM)));
  // most top platform [4]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){WALL_THICKNESS, 900 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){410 / 2400.0, 1400 / 1800.0}, WINDOW_DIM)));
  raise_screen_of_platforms(screen, 1);
}

void add_screen_3(scene_t *scene) {
  list_t *screen = list_init(9, NULL);
  // bottom left platform [0]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){1040 / 2400.0, 200 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1280 / 2400.0, 280 / 1800.0}, WINDOW_DIM)));
  // bottom right platform [1]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){1600 / 2400.0, 200 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1880 / 2400.0, 280 / 1800.0}, WINDOW_DIM)));
  // most right platfotm [2]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){2120 / 2400.0, 440 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1.0 - WALL_THICKNESS, 530 / 1800.0},
                         WINDOW_DIM)));
  // middle stationary platform [3]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){960 / 2400.0, 510 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1640 / 2400.0, 610 / 1800.0}, WINDOW_DIM)));
  // right moving platform [4]
  list_add(
      screen,
      add_moving_platform(
          scene,
          vec_scale((vector_t){1250 / 2400.0, 1140 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1690 / 2400.0, 1250 / 1800.0}, WINDOW_DIM),
          -MOVING_PLATFORM_VELOCITY.y * WINDOW_HEIGHT,
          vec_multiply(WINDOW_HEIGHT, (vector_t){770 / 1800.0, 1250 / 1800.0}),
          'y'));
  // left moving platform [5]
  list_add(
      screen,
      add_moving_platform(
          scene, vec_scale((vector_t){700 / 2400.0, 980 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){920 / 2400.0, 1180 / 1800.0}, WINDOW_DIM),
          MOVING_PLATFORM_VELOCITY.y * WINDOW_HEIGHT,
          vec_multiply(WINDOW_HEIGHT, (vector_t){980 / 1800.0, 1550 / 1800.0}),
          'y'));
  // most right platform [6]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){WALL_THICKNESS, 1230 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){310 / 2400.0, 1310 / 1800.0}, WINDOW_DIM)));
  // top left platform [7]
  list_add(
      screen,
      add_stationary_platform(
          scene, vec_scale((vector_t){690 / 2400.0, 1710 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1030 / 2400.0, 2000 / 1800.0}, WINDOW_DIM)));
  // top right platform [8]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){2250 / 2400.0, 1760 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1.0 - WALL_THICKNESS, 1840 / 1800.0},
                         WINDOW_DIM)));
  raise_screen_of_platforms(screen, 2);
}

void add_screen_4(scene_t *scene) {
  list_t *screen = list_init(8, NULL);
  // bottom left middle platform [0]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){1400 / 2400.0, 520 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1600 / 2400.0, 600 / 1800.0}, WINDOW_DIM)));
  // most left platform [1]
  list_add(screen,
           add_stationary_platform(
               scene,
               vec_scale((vector_t){WALL_THICKNESS, 520 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){200 / 2400.0, 600 / 1800.0}, WINDOW_DIM)));
  // middle right moving platform [2]
  list_add(screen,
           add_moving_platform(
               scene,
               vec_scale((vector_t){2040 / 2400.0, 920 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){2200 / 2400.0, 1000 / 1800.0}, WINDOW_DIM),
               -MOVING_PLATFORM_VELOCITY.y * WINDOW_HEIGHT,
               vec_multiply(WINDOW_WIDTH,
                            (vector_t){1650 / 2400.0, 1.0 - WALL_THICKNESS -
                                                          1.1 * QUEEN_DIM.x}),
               'x'));
  // right step platform [3]
  list_add(
      screen,
      add_stationary_platform(
          scene,
          vec_scale((vector_t){1240 / 2400.0, 1300 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1400 / 2400.0, 1360 / 1800.0}, WINDOW_DIM)));
  // middle step platform [4]
  list_add(
      screen,
      add_disappearing_platform(
          scene, vec_scale((vector_t){930 / 2400.0, 1380 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1090 / 2400.0, 1440 / 1800.0}, WINDOW_DIM)));
  // left step platform [5]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){610 / 2400.0, 1460 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){770 / 2400.0, 1520 / 1800.0}, WINDOW_DIM)));
  // top tall thin platform [6]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){570 / 2400.0, 1460 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){610 / 2400.0, 2420 / 1800.0}, WINDOW_DIM)));
  // top right platform [7]
  list_add(
      screen,
      add_slippery_platform(
          scene,
          vec_scale((vector_t){1020 / 2400.0, 1720 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1860 / 2400.0, 1840 / 1800.0}, WINDOW_DIM)));
  // top most right platform [8]
  list_add(
      screen,
      add_slippery_platform(
          scene,
          vec_scale((vector_t){2200 / 2400.0, 1775 / 1800.0}, WINDOW_DIM),
          vec_scale((vector_t){1.0-WALL_THICKNESS, 1880 / 1800.0}, WINDOW_DIM)));
  raise_screen_of_platforms(screen, 3);
}

void add_screen_5(scene_t *scene) {
  list_t *screen = list_init(8, NULL);
  // bottom right platform [0]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){2150 / 2400.0, 80 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1.0-WALL_THICKNESS, 200 / 1800.0}, WINDOW_DIM)));
  // middle right platform [1]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){2200 / 2400.0, 600 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1.0-WALL_THICKNESS, 750 / 1800.0}, WINDOW_DIM)));
  // middle bottom platform [2]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){760 / 2400.0, 460 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1180 / 2400.0, 590 / 1800.0}, WINDOW_DIM)));
  // bottom left platform [3]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){200 / 2400.0, 300 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){360 / 2400.0, 360 / 1800.0}, WINDOW_DIM)));
  // most middle platform [4]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){830 / 2400.0, 960 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1060 / 2400.0, 1090 / 1800.0}, WINDOW_DIM)));
  // top right platform [5]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){1870 / 2400.0, 1230 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){2000 / 2400.0, 1360 / 1800.0}, WINDOW_DIM)));
  // top right platform [6]
  list_add(screen,
           add_slippery_platform(
               scene,
               vec_scale((vector_t){1400 / 2400.0, 1360 / 1800.0}, WINDOW_DIM),
               vec_scale((vector_t){1630 / 2400.0, 1490 / 1800.0}, WINDOW_DIM)));
  raise_screen_of_platforms(screen, 4);
}

void toggle_pause(state_t *state) {
  if (state->status != PAUSE) {
    state->last_status = state->status;
    state->status = PAUSE;
  } else {
    state->status = state->last_status;
    state->last_status = PAUSE;
  }
}

// direction = 1 is right, direction = -1 is left
void queen_move(body_t *queen, key_event_type_t type, int direction) {
  queen_traits_t *traits =
      (queen_traits_t *)((info_t *)body_get_info(queen))->traits;
  if (type == KEY_PRESSED) {
    traits->last_direction_pressed = direction;
  } else {
    traits->last_direction_pressed = 0;
  }
  if (traits->charging) {
    return;
  }
  if (direction == -1) {
    if (!traits->can_move_left) {
      return;
    }
  } else {
    if (!traits->can_move_right) {
      return;
    }
  }

  // for slippery movement (only when on slippery platform)
  if (((info_t *)body_get_info(traits->movement_medium))->type ==
      SLIPPERY_PLATFORM) {
    if (type == KEY_PRESSED) {
      // get closer to ideal movement
      double movement_force_x = direction * SLIP_ACCELERATION_MAGNITUDE *
                                WINDOW_WIDTH * body_get_mass(queen);
      body_add_force(queen, (vector_t){movement_force_x, 0});
    }
    return;
  }

  // for normal movement
  if (type == KEY_PRESSED) {
    body_set_velocity(
        queen, vec_add(body_get_velocity(traits->movement_medium),
                       (vector_t){direction * QUEEN_SPEED * WINDOW_WIDTH, 0}));
  } else {
    body_set_velocity(queen, body_get_velocity(traits->movement_medium));
  }
}

bool queen_jump(body_t *queen, bool keyed, key_event_type_t type) {
  queen_traits_t *traits =
      (queen_traits_t *)((info_t *)body_get_info(queen))->traits;
  if (traits->in_air) {
    return false;
  }
  if (keyed && type == KEY_PRESSED) {
    if (!traits->charging) {
      traits->charging = true;
      queen_toggle_crouch(queen, true);
      body_set_velocity(queen, body_get_velocity(traits->movement_medium));
      traits->jump_charge_timer = 0;
    }
  }
  if (traits->jump_charge_timer > MAX_JUMP_CHARGE ||
      (keyed && type == KEY_RELEASED)) {
    if (traits->jump_charge_timer > MAX_JUMP_CHARGE) {
      traits->jump_charge_timer = MAX_JUMP_CHARGE;
    }
    double velocity_x =
        traits->last_direction_pressed * JUMP_X_SPEED * WINDOW_WIDTH;
    double velocity_y =
        traits->jump_charge_timer * JUMP_Y_VELOCITY * WINDOW_HEIGHT;
    body_set_velocity(queen, (vector_t){velocity_x, velocity_y});
    body_translate(queen, (vector_t){0, 15 / 1800.0 * WINDOW_HEIGHT});
    traits->jump_charge_timer = 0;
    return true;
  }
  return false;
}

void on_key(char key, key_event_type_t type, double held_time, void *stat) {
  state_t *state = (state_t *)stat;
  if (key == P_KEY && type == KEY_PRESSED) {
    toggle_pause(state);
    return;
  }
  if (key == Q_KEY && type == KEY_PRESSED) {
    printf("EXITED");
    abort();
    return;
  }
  if (state->status != PLAY) {
    return;
  }
  if (key == SPACE_BAR) {
    if (queen_jump(state->queen, true, type))
      return;
  } else {
    if (queen_jump(state->queen, false, type))
      return;
  }
  if (key == RIGHT_ARROW) {
    queen_move(state->queen, type, 1);
  } else if (key == LEFT_ARROW) {
    queen_move(state->queen, type, -1);
  }
}

void win(state_t *state) {
  state->status = WIN;
  body_set_color(state->background, WIN_SCREEN_COLOR);
  body_t *win_star =
      body_init(make_star(WIN_STAR_POINTS, WIN_STAR_INNER_RADIUS,
                          WIN_STAR_OUTER_TO_INNER_RATIO * WIN_STAR_INNER_RADIUS,
                          WINDOW_CENTER, 0),
                0, WIN_STAR_COLOR);
  scene_add_body(state->scene, win_star);
}

bool check_screen_transition(state_t *state) {
  double queen_top_coord = get_coordinate(state->queen, TOP);
  double queen_bottom_coord = get_coordinate(state->queen, BOTTOM);
  if (queen_top_coord > SCREEN_COUNT * WINDOW_HEIGHT) {
    win(state);
    return false;
  }
  double translation_mag = 0;
  if (queen_bottom_coord > WINDOW_HEIGHT) {
    translation_mag = -WINDOW_HEIGHT;
    state->screen_num++;
  } else if (queen_top_coord < 0) {
    translation_mag = WINDOW_HEIGHT;
    state->screen_num--;
  }
  if (translation_mag != 0) {
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *body = scene_get_body(state->scene, i);
      info_t *info = body_get_info(body);
      if (info->type == BACKGROUND) {
        continue;
      }
      body_translate(body, (vector_t){0, translation_mag});
      if (info->type == MOVING_Y_PLATFORM) {
        platform_traits_t *traits = (platform_traits_t *)info->traits;
        vector_t new_bounds = {traits->bounds.x + translation_mag,
                               traits->bounds.y + translation_mag};
        traits->bounds = new_bounds;
      }
    }
    return true;
    // body_set_color(state->background, r_color());
  }
  return false;
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){WINDOW_WIDTH, WINDOW_HEIGHT});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->status = PLAY;
  state->screen_num = 1;
  add_background(state);
  add_walls(state->scene);
  add_screen_1(state->scene);
  add_screen_2(state->scene);
  add_screen_3(state->scene);
  add_screen_4(state->scene);
  add_screen_5(state->scene);
  add_queen_hitbox(state);
  scene_add_bodies_force_creator(state->scene, conditional_gravity, state, NULL,
                                 (free_func_t)free);
  while (check_screen_transition(state)) {
    ;
  }
  make_platform_collisions(state);
  sdl_on_key((void *)on_key);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (state->status == PLAY) {
    control_special_platform_behavior(state, dt);
    queen_zero_flags(state->queen);
    scene_tick(state->scene, dt);
    check_screen_transition(state);
    queen_traits_t *traits = ((info_t *)body_get_info(state->queen))->traits;
    if (traits->charging) {
      traits->jump_charge_timer += dt;
    }
    // for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    //   body_t *body = scene_get_body(state->scene, i);
    //   info_t *info = body_get_info(body);
    //   if (info->type == MOVING_X_PLATFORM || info->type == MOVING_Y_PLATFORM)
    //   {
    //     if (platform_outside_boundaries(body)) {
    //       body_set_velocity(body, vec_negate(body_get_velocity(body)));
    //     }
    //   }
    // }
  }
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}