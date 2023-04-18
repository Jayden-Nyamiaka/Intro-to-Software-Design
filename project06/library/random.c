#include "random.h"

int r_sign() { return rand() % 2 == 0 ? 1 : -1; }

int r_int(int min, int max) { return min + rand() % (max - min + 1); }

double r_double(double min, double max) {
  return min + ((rand() * 1.0 / RAND_MAX) * (max - min));
}

rgb_color_t r_color() {
  return (rgb_color_t){r_double(0, 1), r_double(0, 1), r_double(0, 1)};
}

rgb_color_t r_pastel_color() {
  rgb_color_t color = r_color();
  color = (rgb_color_t){0.5 * (color.r + 1), 0.5 * (color.g + 1),
                        0.5 * (color.b + 1)};
  return color;
}