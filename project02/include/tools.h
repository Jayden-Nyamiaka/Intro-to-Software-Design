#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "polygon.h"
#include "vec_list.h"

typedef struct color {
  float red;
  float green;
  float blue;
} color_t;

typedef struct screen {
  color_t color; // pastel purple - Dilichi
  vec_list_t *points;
} background;

typedef struct shape {
  size_t sides;
  color_t color;
  vector_t velocity;
  double rotation;
  double elasticity;
  vec_list_t *points;
} star;

typedef struct shape_list star_list;
star_list *star_list_init(size_t initial_capacity);
size_t star_list_size(star_list *lst);
// returns null if no value at index
star *star_list_get(star_list *lst, size_t index);
void star_list_free(star_list *lst);
void star_list_add(star_list *lst, star *value);
star *star_list_remove(star_list *lst, size_t index);

int r_sign();
double r_double();
int r_int(int min, int max);
color_t r_color();

background *make_background(color_t color);

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
star *make_star(size_t sides, double inner_r, double outer_r, double angle,
                vector_t start_pos);
int star_check_remove(star *s);
void star_update(star *s, double dt);

void free_star(star *s);
void free_background(background *bg);

#endif // #ifndef __TOOLS_H__
