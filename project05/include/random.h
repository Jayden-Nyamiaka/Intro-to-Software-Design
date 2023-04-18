#include "color.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Returns a random sign
 *
 * @return int the sign returned (1 or -1)
 */
int r_sign();

/**
 * @brief returns a random int between min (inclusive) and max (inclusive)
 *
 * @param min integer where random number gen starts
 * @param max integer where random number gen ends
 * @return double the random int returned [min, max]
 */
int r_int(int min, int max);

/**
 * @brief returns a random double between min (inclusive) and max (inclusive)
 *
 * @param min double where random number gen starts
 * @param max double where random number gen ends
 * @return double the random double returned [min, max]
 */
double r_double(double min, double max);

/**
 * @brief returns a random color with all random rgb floats
 *
 * @return rgb_color_t the random color returned
 */
rgb_color_t r_color();

/**
 * @brief returns a random pastel color with all random rgb floats
 *
 * @return rgb_color_t the random pastel color returned
 */
rgb_color_t r_pastel_color();