#include "forces.h"
#include <stdlib.h>

const double BLOW_UP_DISTANCE = 5; // 5 for tests | 8 for preference
const size_t INITIAL_AUXES_SIZE = 10;

typedef struct auxiliary {
  double constant;
  list_t *bodies;
} auxiliary_t;

list_t *aux_get_bodies(void *aux) { return ((auxiliary_t *)aux)->bodies; }

auxiliary_t *auxiliary_init(double constant, list_t *bodies) {
  auxiliary_t *aux = malloc(sizeof(auxiliary_t));
  aux->constant = constant;
  aux->bodies = bodies;
  return aux;
}

void auxiliary_free(auxiliary_t *aux) {
  list_free(aux->bodies);
  free(aux);
}

typedef struct force_applier {
  force_creator_t forcer;
  list_t *auxes;
} force_applier_t;

force_applier_t *force_applier_init(force_creator_t forcer, free_func_t freer) {
  force_applier_t *applier = malloc(sizeof(force_applier_t));
  applier->forcer = forcer;
  applier->auxes = list_init(INITIAL_AUXES_SIZE, freer);
  return applier;
}

void force_applier_free(force_applier_t *applier) {
  list_free(applier->auxes);
  free(applier);
}

size_t force_applier_auxes(force_applier_t *applier) {
  return list_size(applier->auxes);
}

force_creator_t force_applier_get_force_creator(force_applier_t *applier) {
  return applier->forcer;
}

void force_applier_add_aux(force_applier_t *applier, void *aux) {
  list_add(applier->auxes, aux);
}

void *force_applier_get_aux(force_applier_t *applier, size_t index) {
  return list_get(applier->auxes, index);
}

void force_applier_remove_aux(force_applier_t *applier, size_t index) {
  auxiliary_free((auxiliary_t *)list_remove(applier->auxes, index));
}

void apply_earth_gravity(void *aux) {
  auxiliary_t *auxil = (auxiliary_t *)aux;
  double g = auxil->constant;
  double mass = (double)body_get_mass(list_get(auxil->bodies, 0));
  vector_t force = {0, -1 * mass * g};
  body_add_force(list_get(auxil->bodies, 0), force);
}

void create_earth_gravity(scene_t *scene, double g, list_t *bodies) {
  scene_add_bodies_force_creator(scene, apply_earth_gravity,
                                 auxiliary_init(g, bodies), bodies,
                                 (free_func_t)auxiliary_free);
}

void apply_newtonian_gravity(void *aux) {
  auxiliary_t *auxil = (auxiliary_t *)aux;
  body_t *body_1 = (body_t *)list_get(auxil->bodies, 0);
  body_t *body_2 = (body_t *)list_get(auxil->bodies, 1);
  vector_t centroid_1 = body_get_centroid(body_1);
  vector_t centroid_2 = body_get_centroid(body_2);
  double distance = vec_distance(centroid_1, centroid_2);
  if (distance < BLOW_UP_DISTANCE) {
    return;
  }
  double G = auxil->constant;
  double mass_1 = (double)body_get_mass(body_1);
  double mass_2 = (double)body_get_mass(body_2);
  double force_mag = G * mass_1 * mass_2 / (distance * distance);
  vector_t unit_vec = vec_unit(vec_subtract(centroid_1, centroid_2));
  vector_t force = vec_multiply(force_mag, unit_vec);
  body_add_force(body_1, vec_negate(force));
  body_add_force(body_2, force);
}

void create_newtonian_gravity(scene_t *scene, double G, list_t *bodies) {
  scene_add_bodies_force_creator(scene, apply_newtonian_gravity,
                                 auxiliary_init(G, bodies), bodies,
                                 (free_func_t)auxiliary_free);
}

void apply_spring(void *aux) {
  auxiliary_t *auxil = (auxiliary_t *)aux;
  body_t *body_1 = (body_t *)list_get(auxil->bodies, 0);
  body_t *body_2 = (body_t *)list_get(auxil->bodies, 1);
  vector_t centroid_1 = body_get_centroid(body_1);
  vector_t centroid_2 = body_get_centroid(body_2);
  double k = auxil->constant;
  vector_t displacement = vec_subtract(centroid_2, centroid_1);
  vector_t force = vec_multiply(-1 * k, displacement);
  body_add_force(body_1, vec_negate(force));
  body_add_force(body_2, force);
}

void create_spring(scene_t *scene, double k, list_t *bodies) {
  scene_add_bodies_force_creator(scene, apply_spring, auxiliary_init(k, bodies),
                                 bodies, (free_func_t)auxiliary_free);
}

void apply_drag(void *aux) {
  auxiliary_t *auxil = (auxiliary_t *)aux;
  body_t *body = (body_t *)list_get(auxil->bodies, 0);
  double gamma = auxil->constant;
  vector_t velocity = body_get_velocity(body);
  vector_t force = vec_multiply(-1 * gamma, velocity);
  body_add_force(body, force);
}

void create_drag(scene_t *scene, double gamma, list_t *bodies) {
  scene_add_bodies_force_creator(scene, apply_drag,
                                 auxiliary_init(gamma, bodies), bodies,
                                 (free_func_t)auxiliary_free);
}

void apply_destructive_collision(void *aux) {
  auxiliary_t *auxil = (auxiliary_t *)aux;
  body_t *body_1 = (body_t *)list_get(auxil->bodies, 0);
  body_t *body_2 = (body_t *)list_get(auxil->bodies, 1);
  list_t *shape_1 = body_get_shape(body_1);
  list_t *shape_2 = body_get_shape(body_2);
  if (find_collision(shape_1, shape_2)) {
    body_remove(body_1);
    body_remove(body_2);
  }
  list_free(shape_1);
  list_free(shape_2);
}

void create_destructive_collision(scene_t *scene, list_t *bodies) {
  scene_add_bodies_force_creator(scene, apply_destructive_collision,
                                 auxiliary_init(0, bodies), bodies,
                                 (free_func_t)auxiliary_free);
}