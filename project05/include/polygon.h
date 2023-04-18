#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "list.h"
#include "vector.h"

/**
 * Computes the area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the area of the polygon
 */
double polygon_area(list_t *polygon);

/**
 * Computes the center of mass of a polygon.
 * See https://en.wikipedia.org/wiki/Centroid#Of_a_polygon.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the centroid of the polygon
 */
vector_t polygon_centroid(list_t *polygon);

/**
 * Translates all vertices in a polygon by a given vector.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param translation the vector to add to each vertex's position
 */
void polygon_translate(list_t *polygon, vector_t translation);

/**
 * Rotates vertices in a polygon by a given angle about a given point.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param angle the angle to rotate the polygon, in radians.
 * A positive angle means counterclockwise.
 * @param point the point to rotate around
 */
void polygon_rotate(list_t *polygon, double angle, vector_t point);

/**
 * Makes a list_t * of vector_t's that assign the points for a
 * 'points'-sided shape with radius of size 'radius' with center
 * at (0,0)
 *
 * @param radius length of radius from center of shape
 * @param points number of vectors the polygon should have
 * @return list_t* with vectors of shape
 */
list_t *make_closed_polygon(double radius, int points);

/**
 * Makes a 'points'-pointed star shape with outer radius
 * length of 'radius' and a center at position center;
 *
 * @param points number of points on star
 * @param inner_radius length of inner radius of star
 * @param outer_radius length of outer radius of star
 * @param center the position of the star's center shape
 * @param angle angle of orientation of the shape in radians
 * @return list_t* with vectors of star
 */
list_t *make_star(int points, double inner_radius, double outer_radius,
                  vector_t center, double angle);

/**
 * Makes a rectangle with corners at corner_one and corner_two
 *
 * @param corner_one The position of one corner of rectangle
 * @param corner_two The position of second corner of rectangle
 * @return list_t* with vectors of rectangle
 */
list_t *make_rectangle(vector_t corner_one, vector_t corner_two);

/**
 * Makes a circle with 'radius' at 'center' with points
 * defined by 'dots per radius'
 *
 * @param radius The radius of the circle
 * @param dots_per_radius The dots per radius ratio of the circle
 * @param center The center of the circle
 * @return list_t* with vectors of circle
 */
list_t *make_circle(double radius, double dots_per_radius, vector_t center);

#endif // #ifndef __POLYGON_H__
