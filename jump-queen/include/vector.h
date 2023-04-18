#ifndef __VECTOR_H__
#define __VECTOR_H__

/**
 * A real-valued 2-dimensional vector.
 * Positive x is towards the right; positive y is towards the top.
 * vector_t is defined here instead of vector.c because it is passed *by value*.
 */
typedef struct {
  double x;
  double y;
} vector_t;

/**
 * The zero vector, i.e. (0, 0).
 * "extern" declares this global variable without allocating memory for it.
 * You will need to define "const vector_t VEC_ZERO = ..." in vector.c.
 */
extern const vector_t VEC_ZERO;

/**
 * Checks if two vectors are equal.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return 1 if equal, 0 if otherwise
 */
int vec_equal(vector_t v1, vector_t v2);

/**
 * Adds two vectors.
 * Performs the usual componentwise vector sum.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 + v2
 */
vector_t vec_add(vector_t v1, vector_t v2);

/**
 * Subtracts two vectors.
 * Performs the usual componentwise vector difference.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 - v2
 */
vector_t vec_subtract(vector_t v1, vector_t v2);

/**
 * Computes the additive inverse a vector.
 * This is equivalent to multiplying by -1.
 *
 * @param v the vector whose inverse to compute
 * @return -v
 */
vector_t vec_negate(vector_t v);

/**
 * Multiplies a vector by a scalar.
 * Performs the usual componentwise product.
 *
 * @param scalar the number to multiply the vector by
 * @param v the vector to scale
 * @return scalar * v
 */
vector_t vec_multiply(double scalar, vector_t v);

/**
 * Scales the vector via each other.
 * Returns a vector with multiplied the x and y components.
 * Note commutativity: vec_scale(a, b) == vec_scale(b, a).
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 - v2
 */
vector_t vec_scale(vector_t v1, vector_t v2);

/**
 * Computes the dot product of two vectors.
 * See https://en.wikipedia.org/wiki/Dot_product#Algebraic_definition.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 . v2
 */
double vec_dot(vector_t v1, vector_t v2);

/**
 * Computes the cross product of two vectors,
 * which lies along the z-axis.
 * See https://en.wikipedia.org/wiki/Cross_product#Computing_the_cross_product.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the z-component of v1 x v2
 */
double vec_cross(vector_t v1, vector_t v2);

/**
 * Rotates a vector by an angle around (0, 0).
 * The angle is given in radians.
 * Positive angles are counterclockwise, according to the right hand rule.
 * See https://en.wikipedia.org/wiki/Rotation_matrix.
 * (You can derive this matrix by noticing that rotation by a fixed angle
 * is linear and then computing what it does to (1, 0) and (0, 1).)
 *
 * @param v the vector to rotate
 * @param angle the angle to rotate the vector
 * @return v rotated by the given angle
 */
vector_t vec_rotate(vector_t v, double angle);

/**
 * Computes the distance between two vectors.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the unsigned distance between v1 and v2
 */
double vec_distance(vector_t v1, vector_t v2);

/**
 * Computes the given vector as a unit vector.
 *
 * @param v1 the vector to compute
 * @return the unit vector of v
 */
vector_t vec_unit(vector_t v);

/**
 * Computes the perpendicular unit vector axis of a vector from point1 to
 * point2.
 *
 * @param point1 the first point where the axis comes from
 * @param point2 the second point that the axis goes towards
 * @return the perpendicular unit vector axis from point1 to point2
 */
vector_t vec_perpendicular_unit_axis(vector_t point1, vector_t point2);

#endif // #ifndef __VECTOR_H__
