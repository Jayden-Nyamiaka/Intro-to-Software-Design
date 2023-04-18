#include "state.h"
#include "tools.h"
#include <assert.h>
#include <time.h>

const vector_t MIN_WINDOW = {0.0, 0.0};
const vector_t MAX_WINDOW = {1000.0, 500.0};

const size_t INITIAL_CAPACITY = 7;
const int STAR_TIME_INTERVAL = 2; // in seconds
const double INNER_RADIUS = 15;
const double OUTER_RADIUS = 40;
const color_t BACKGROUND_COLOR = {252 / 255.0, 243 / 255.0, 255.0 / 255.0};

typedef struct state {
  size_t sides;
  double time_since_star;
  background *bg;
  star_list *stars;
} state_t;

state_t *emscripten_init() {
  srand(time(NULL));
  state_t *state = malloc(sizeof(state_t));
  state->sides = 2;
  state->time_since_star = 1000;
  state->bg = make_background(BACKGROUND_COLOR);
  state->stars = star_list_init(INITIAL_CAPACITY);
  sdl_init(MIN_WINDOW, MAX_WINDOW);
  return state;
}

void emscripten_main(state_t *state) {
  sdl_clear();
  double dt = time_since_last_tick();
  state->time_since_star += dt;
  
  if (state->time_since_star > STAR_TIME_INTERVAL) {
    vector_t start_pos = {MIN_WINDOW.x + OUTER_RADIUS, MAX_WINDOW.y - OUTER_RADIUS};
    star_list_add(state->stars, make_star(state->sides, INNER_RADIUS, OUTER_RADIUS, 0, start_pos));
    state->sides++;
    state->time_since_star = 0;
  }
  sdl_draw_polygon(state->bg->points, state->bg->color.red, 
                                state->bg->color.green, state->bg->color.blue);
  for (size_t i = 0; i < star_list_size(state->stars); i++) {
    star *current = star_list_get(state->stars, i);
    star_update(current, dt);
    sdl_draw_polygon(current->points, current->color.red, current->color.green,
                     current->color.blue);
    if (star_check_remove(current)) {
      free_star(star_list_remove(state->stars, i));
    }
  }
  sdl_show();
}

void emscripten_free(state_t *state) {
  star_list_free(state->stars);
  free_background(state->bg);
  free(state);
}
