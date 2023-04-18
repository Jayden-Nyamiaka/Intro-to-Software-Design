#include "state.h"
#include <assert.h>
#include <time.h>
#define PI (3.14159265358979323846)

/* for project requirement:
  STAR_SIZE = {20, 50}
  STAR_NUM = 1
  STAR_SIDES = 5
*/
const vector_t STAR_SIZE = (vector_t){15, 30};
const size_t STAR_NUM = 5;
const size_t STAR_SIDES = 5;
const vector_t MIN_WINDOW = {0.0, 0.0};
const vector_t MAX_WINDOW = {1000.0, 500.0};
const vector_t SPEED_BOUNDS = {75, 550};
const vector_t ROTATION_BOUNDS = {1, 10};

int r_sign() { return rand() % 2 == 0 ? 1 : -1; }
double r_double() { return rand() * 1.0 / RAND_MAX; }
int r_int(int min, int max) {
  return min + rand() % (max - min + 1);
} // both inclusive

vec_list_t *make_star(size_t sides, double inner_r, double outer_r,
                      double angle, vector_t start_pos);
void new_behavior(state_t *state, size_t star_index);
void new_color(state_t *state, size_t star_index);
void new_speed(state_t *state, size_t star_index);
void new_rotation(state_t *state, size_t star_index);

typedef struct color {
  float red;
  float green;
  float blue;
} color_t;

typedef struct state {
  vec_list_t **stars;
  vector_t *directions;
  color_t *colors;
  double *speeds;
  double *rotations;

} state_t;

state_t *emscripten_init() {
  srand(time(NULL));
  state_t *cur_state = malloc(sizeof(state_t));

  cur_state->speeds = malloc(STAR_NUM * sizeof(double));
  cur_state->colors = malloc(STAR_NUM * sizeof(color_t));
  cur_state->directions = malloc(STAR_NUM * sizeof(vector_t));
  cur_state->rotations = malloc(STAR_NUM * sizeof(double));

  for (size_t i = 0; i < STAR_NUM; i++) {
    new_speed(cur_state, i);
    new_color(cur_state, i);
    cur_state->directions[i] = (vector_t){r_sign(), r_sign()};
    cur_state->rotations[i] =
        r_sign() * r_int(ROTATION_BOUNDS.x, ROTATION_BOUNDS.y);
  }

  cur_state->stars = malloc(STAR_NUM * sizeof(vec_list_t *));
  size_t partitions = 50;
  double portion = 0.85;
  vector_t dimensions = vec_subtract(MAX_WINDOW, MIN_WINDOW);
  for (size_t i = 0; i < STAR_NUM; i++) {
    vector_t random_offset = vec_multiply(
        portion * (rand() % partitions) / (partitions - 1), dimensions);
    vector_t random_position =
        vec_add(vec_multiply(0.5 - portion / 2, dimensions), random_offset);
    cur_state->stars[i] =
        make_star(STAR_SIDES, STAR_SIZE.x, STAR_SIZE.y, 0, random_position);
  }

  sdl_init(MIN_WINDOW, MAX_WINDOW);
  return cur_state;
}

void new_color(state_t *state, size_t star_index) {
  state->colors[star_index] = (color_t){r_double(), r_double(), r_double()};
}

void new_speed(state_t *state, size_t star_index) {
  state->speeds[star_index] = r_int(SPEED_BOUNDS.x, SPEED_BOUNDS.y);
}

void new_rotation(state_t *state, size_t star_index) {
  state->rotations[star_index] = r_int(ROTATION_BOUNDS.x, ROTATION_BOUNDS.y) *
                                 (state->rotations[star_index] > 0 ? 1 : -1);
}

void new_behavior(state_t *state, size_t star_index) {
  new_color(state, star_index);
  new_speed(state, star_index);
  new_rotation(state, star_index);
}

/**
 * @brief Generates the points for a star polygon
 *
 * @param sides         number of sides
 * @param inner_r       inner radius of the star
 * @param outer_r       outer radius of the star
 * @param angle         angle of orientation
 * @param start_pos     starting position of the center of the star
 * @return vec_list_t*  points of the star polygon
 */
vec_list_t *make_star(size_t sides, double inner_r, double outer_r,
                      double angle, vector_t start_pos) {
  assert(sides > 1 && outer_r > inner_r &&
         (start_pos.x > MIN_WINDOW.x || start_pos.x < MAX_WINDOW.x) &&
         (start_pos.y > MIN_WINDOW.y || start_pos.y < MAX_WINDOW.y));
  vec_list_t *points = vec_list_init(2 * sides);
  for (size_t i = 0; i < sides; i++) {
    vector_t *temp = malloc(sizeof(vector_t));
    double temp_angle = (i * 2.0 * PI) / sides;
    temp->x = outer_r * cos(temp_angle) + start_pos.x;
    temp->y = outer_r * sin(temp_angle) + start_pos.y;
    vec_list_add(points, temp);
    temp = malloc(sizeof(vector_t));
    temp_angle += PI / sides;
    temp->x = inner_r * cos(temp_angle) + start_pos.x;
    temp->y = inner_r * sin(temp_angle) + start_pos.y;
    vec_list_add(points, temp);
  }
  return points;
}

void update_stars(state_t *state, double dt) {
  for (size_t n = 0; n < STAR_NUM; n++) {
    vector_t update_pos =
        vec_multiply(dt * state->speeds[n], state->directions[n]);
    polygon_translate(state->stars[n], update_pos);
    polygon_rotate(state->stars[n], dt * state->rotations[n],
                   polygon_centroid(state->stars[n]));
  }
}

void check_collisions(state_t *state) {
  for (size_t n = 0; n < STAR_NUM; n++) {
    int changed_x = 0;
    int changed_y = 0;
    for (size_t i = 0; i < vec_list_size(state->stars[n]); i += 2) {
      vector_t *current = vec_list_get(state->stars[n], i);
      if (!changed_x &&
          (current->x >= MAX_WINDOW.x || current->x <= MIN_WINDOW.x)) {
        if (current->x >= MAX_WINDOW.x) {
          state->directions[n].x = -1;
        } else {
          state->directions[n].x = 1;
        }
        state->rotations[n] *= (-1);
        new_behavior(state, n);
        changed_x = 1;
      }
      if (!changed_y &&
          (current->y >= MAX_WINDOW.y || current->y <= MIN_WINDOW.y)) {
        if (current->y >= MAX_WINDOW.y) {
          state->directions[n].y = -1;
        } else {
          state->directions[n].y = 1;
        }
        state->rotations[n] *= (-1);
        new_behavior(state, n);
        changed_y = 1;
      }
      if (changed_x && changed_y) {
        break;
      }
    }
  }
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  sdl_clear();
  update_stars(state, dt);
  check_collisions(state);
  for (size_t i = 0; i < STAR_NUM; i++) {
    sdl_draw_polygon(state->stars[i], state->colors[i].red,
                     state->colors[i].green, state->colors[i].blue);
  }
  sdl_show();
}

void emscripten_free(state_t *state) {
  for (size_t i = 0; i < STAR_NUM; i++) {
    vec_list_free(state->stars[i]);
  }
  free(state->stars);
  free(state->directions);
  free(state->colors);
  free(state->speeds);
  free(state->rotations);
  free(state);
}
