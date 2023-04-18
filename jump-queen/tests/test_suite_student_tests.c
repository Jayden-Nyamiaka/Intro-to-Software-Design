// TODO: IMPLEMENT YOUR TESTS IN THIS FILE
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "forces.h"
#include "polygon.h"
#include "random.h"
#include "test_util.h"

list_t *make_square() {
  list_t *sq = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){+1, +1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, -1};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(sq, v);
  return sq;
}

void test_random_spring_sinusoid() {
  double m = r_double(10, 100);
  double k = r_double(1, 10);
  double a = r_double(1, 10);
  int steps = r_int(500000, 1000000);
  double dt = 1 / steps;
  scene_t *scene = scene_init();
  body_t *body = body_init(make_square(), m, (rgb_color_t){0, 0, 0});
  body_set_centroid(body, (vector_t){a, 0});
  scene_add_body(scene, body);
  body_t *anchor = body_init(make_square(), INFINITY, (rgb_color_t){0, 0, 0});
  scene_add_body(scene, anchor);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body);
  list_add(bodies, anchor);
  create_spring(scene, k, bodies);
  for (int i = 0; i < steps; i++) {
    assert(vec_isclose(body_get_centroid(body),
                       (vector_t){a * cos(sqrt(k / m) * i * dt), 0}));
    assert(vec_equal(body_get_centroid(anchor), VEC_ZERO));
    scene_tick(scene, dt);
  }
  scene_free(scene);
}

double gravity_potential(double G, body_t *body1, body_t *body2) {
  vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}
double kinetic_energy(body_t *body) {
  vector_t v = body_get_velocity(body);
  return body_get_mass(body) * vec_dot(v, v) / 2;
}

void test_random_energy_conservation() {
  double m1 = r_double(3, 10);
  double m2 = r_double(5, 15);
  double G = r_double(10, 1000);
  int steps = r_int(500000, 1000000);
  double dt = 1 / steps;
  scene_t *scene = scene_init();
  body_t *body1 = body_init(make_square(), m1, (rgb_color_t){0, 0, 0});
  body_t *body2 = body_init(make_square(), m2, (rgb_color_t){0, 0, 0});
  body_set_centroid(body2, (vector_t){10, 20});
  scene_add_body(scene, body1);
  scene_add_body(scene, body2);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  create_newtonian_gravity(scene, G, bodies);
  double initial_e = gravity_potential(G, body1, body2);
  for (int i = 0; i < steps; i++) {
    assert(body_get_centroid(body1).x < body_get_centroid(body2).x);
    double energy = gravity_potential(G, body1, body2) + kinetic_energy(body1) +
                    kinetic_energy(body2);
    assert(within(.0001, energy / initial_e, 1));
    scene_tick(scene, dt);
  }
  scene_free(scene);
}

double get_distance(vector_t v1, vector_t v2) {
  vector_t diff = vec_subtract(v1, v2);
  return sqrt((diff.x * diff.x) + (diff.y * diff.y));
}

const double GRAVITY_CONSTANT = 6.673e-11;

void test_random_orbit() {
  double m1 = r_double(5, 10);
  double m2 = r_double(100000, 500000);
  double G = r_double(10, 20);
  int steps = r_int(500000, 1000000);
  double dt = 3 / steps;
  scene_t *scene = scene_init();
  body_t *body1 = body_init(make_square(), m1, (rgb_color_t){0, 0, 0});
  body_t *body2 = body_init(make_square(), m2, (rgb_color_t){0, 0, 0});
  body_set_centroid(body1, (vector_t){0, 15});
  body_set_velocity(body1, (vector_t){sqrt(GRAVITY_CONSTANT * m2 / 15), 0});
  scene_add_body(scene, body1);
  scene_add_body(scene, body2);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  create_newtonian_gravity(scene, G, bodies);
  for (int i = 0; i < steps; i++) {
    double dist =
        get_distance(body_get_centroid(body1), body_get_centroid(body2));
    assert(14 < dist && dist < 16);
    scene_tick(scene, dt);
  }
  scene_free(scene);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_random_spring_sinusoid)
  DO_TEST(test_random_energy_conservation)
  DO_TEST(test_random_orbit);

  puts("student_test PASS");
}