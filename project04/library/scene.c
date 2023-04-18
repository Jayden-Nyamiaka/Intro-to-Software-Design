#include "stdlib.h"
#include "stdio.h"

#include "scene.h"
#include "assert.h"
#include "body.h"
#include "forces.h"

const size_t BODIES_INTIAL_CAPACITY = 25;
const size_t FORCES_INTIAL_CAPACITY = 500;

typedef struct scene {
  list_t *bodies;
  list_t *forces;
} scene_t;

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  scene->bodies = list_init(BODIES_INTIAL_CAPACITY, (free_func_t)body_free);
  assert(scene->bodies != NULL);
  scene->forces = list_init(FORCES_INTIAL_CAPACITY, (free_func_t)force_holder_free);
  return scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->forces);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

size_t scene_forces(scene_t *scene) { return list_size(scene->forces); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_free(list_remove(scene->bodies, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  force_holder_t *holder = force_holder_init(forcer, aux, freer);
  list_add(scene->forces, holder);
}

void scene_tick(scene_t *scene, double dt) {
  // run through and calculate every force_holder
  for (size_t i = 0; i < list_size(scene->forces); i++) {
    force_holder_t *holder = (force_holder_t *)list_get(scene->forces, i);
    (force_holder_get_force_creator(holder))(force_holder_get_aux(holder));
  }
  for (int i = 0; i < scene_bodies(scene); i++) {
    body_tick(list_get(scene->bodies, i), dt);
  }
}