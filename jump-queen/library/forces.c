#include "forces.h"
#include "collision.h"
#include <math.h>
#include <stdlib.h>

const double BLOW_UP_DISTANCE = 5; // 5 for tests | 8 for preference
const size_t INITIAL_AUXES_SIZE = 10;

typedef struct auxiliary {
  double constant;
  list_t *bodies;
} auxiliary_t;

auxiliary_t *auxiliary_init(double constant, list_t *bodies) {
  auxiliary_t *aux = malloc(sizeof(auxiliary_t));
  aux->constant = constant;
  aux->bodies = bodies;
  return aux;
}

double aux_get_constant(auxiliary_t *aux) { return aux->constant; }

void auxiliary_free(auxiliary_t *aux) {
  list_free(aux->bodies);
  free(aux);
}

typedef struct collision_aux {
  double elasticity;
} physics_collision_aux_t;

void *physics_collision_aux_init(double elasticity) {
  physics_collision_aux_t *aux = malloc(sizeof(physics_collision_aux_t));
  aux->elasticity = elasticity;
  return aux;
}

typedef struct auxiliary_collision {
  void *aux;
  list_t *bodies;
  collision_handler_t handler;
  bool last_tick_collision;
  free_func_t freer;
} auxiliary_collision_t;

auxiliary_collision_t *auxiliary_collision_init(auxiliary_t *aux,
                                                list_t *bodies,
                                                collision_handler_t handler,
                                                free_func_t freer) {
  auxiliary_collision_t *aux_collision = malloc(sizeof(auxiliary_collision_t));
  aux_collision->aux = aux;
  aux_collision->bodies = bodies;
  aux_collision->handler = handler;
  aux_collision->last_tick_collision = false;
  aux_collision->freer = freer;
  return aux_collision;
}

void auxiliary_collision_free(auxiliary_collision_t *aux_collision) {
  if (aux_collision->freer != NULL) {
    aux_collision->freer(aux_collision->aux);
  }
  free(aux_collision);
}

list_t *aux_get_bodies(void *auxil, free_func_t freer) {
  if (freer == (free_func_t)free) {
    return NULL;
  }
  if (freer == (free_func_t)auxiliary_free) {
    return ((auxiliary_t *)auxil)->bodies;
  }
  return ((auxiliary_collision_t *)auxil)->bodies;
}

typedef struct force_applier {
  force_creator_t forcer;
  list_t *auxes;
  free_func_t freer;
} force_applier_t;

force_applier_t *force_applier_init(force_creator_t forcer, free_func_t freer) {
  force_applier_t *applier = malloc(sizeof(force_applier_t));
  applier->forcer = forcer;
  applier->auxes = list_init(INITIAL_AUXES_SIZE, freer);
  applier->freer = freer;
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

free_func_t force_applier_get_freer(force_applier_t *applier) {
  return applier->freer;
}

void force_applier_add_aux(force_applier_t *applier, void *aux) {
  list_add(applier->auxes, aux);
}

void *force_applier_get_aux(force_applier_t *applier, size_t index) {
  return list_get(applier->auxes, index);
}

void force_applier_remove_aux(force_applier_t *applier, size_t index) {
  applier->freer(list_remove(applier->auxes, index));
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

void apply_collision(void *aux) {
  auxiliary_collision_t *auxil = (auxiliary_collision_t *)aux;
  body_t *body_1 = (body_t *)list_get(auxil->bodies, 0);
  body_t *body_2 = (body_t *)list_get(auxil->bodies, 1);
  list_t *shape_1 = body_get_shape(body_1);
  list_t *shape_2 = body_get_shape(body_2);
  collision_info_t collision_info = find_collision(shape_1, shape_2);
  list_free(shape_1);
  list_free(shape_2);
  if (!collision_info.collided) {
    auxil->last_tick_collision = false;
    return;
  }
  auxil->handler(auxil->last_tick_collision, body_1, body_2,
                 collision_info.axis, auxil->aux);
  auxil->last_tick_collision = true;
}

void handle_destructive_collision(bool last_tick_collision, body_t *body1,
                                  body_t *body2, vector_t axis, void *aux) {
  if (last_tick_collision) {
    return;
  }
  if (body_is_removable(body1)) {
    body_remove(body1);
  }
  if (body_is_removable(body2)) {
    body_remove(body2);
  }
}

void handle_physics_collision(bool last_tick_collision, body_t *body1,
                              body_t *body2, vector_t axis, void *aux) {
  if (last_tick_collision) {
    return;
  }
  double elasticity = ((physics_collision_aux_t *)aux)->elasticity;
  double mass1 = body_get_mass(body1);
  double mass2 = body_get_mass(body2);
  if (mass1 == INFINITY && mass2 == INFINITY) {
    return;
  }
  double vel_1_along_axis = vec_dot(body_get_velocity(body1), axis);
  double vel_2_along_axis = vec_dot(body_get_velocity(body2), axis);
  double component = (1 + elasticity) * (vel_2_along_axis - vel_1_along_axis);
  if (mass2 == INFINITY) {
    vector_t impulse = vec_multiply(mass1 * component, axis);
    body_add_impulse(body1, impulse);
    return;
  }
  if (mass1 == INFINITY) {
    vector_t impulse = vec_multiply(-mass2 * component, axis);
    body_add_impulse(body2, impulse);
    return;
  }
  double reduced_mass = mass1 * mass2 / (mass1 + mass2);
  vector_t impulse = vec_multiply(reduced_mass * component, axis);
  body_add_impulse(body1, impulse);
  body_add_impulse(body2, vec_negate(impulse));
}

void create_destructive_collision(scene_t *scene, list_t *bodies) {
  create_collision(scene, bodies, handle_destructive_collision, NULL, NULL);
}

void create_physics_collision(scene_t *scene, double elasticity,
                              list_t *bodies) {
  void *aux = physics_collision_aux_init(elasticity);
  create_collision(scene, bodies, handle_physics_collision, aux, free);
}

void create_collision(scene_t *scene, list_t *bodies,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  auxiliary_collision_t *aux_collision =
      auxiliary_collision_init(aux, bodies, handler, freer);
  scene_add_bodies_force_creator(scene, apply_collision, aux_collision, bodies,
                                 (free_func_t)auxiliary_collision_free);
}