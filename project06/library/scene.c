#include "stdio.h"
#include "stdlib.h"

#include "assert.h"
#include "body.h"
#include "forces.h"
#include "scene.h"

const size_t BODIES_INTIAL_CAPACITY = 25;
const size_t FORCE_APPLIERS_INTIAL_CAPACITY = 3;

typedef struct scene {
  list_t *bodies;
  list_t *force_appliers;
} scene_t;

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  scene->bodies = list_init(BODIES_INTIAL_CAPACITY, (free_func_t)body_free);
  assert(scene->bodies != NULL);
  scene->force_appliers = list_init(FORCE_APPLIERS_INTIAL_CAPACITY,
                                    (free_func_t)force_applier_free);
  assert(scene->force_appliers != NULL);
  return scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->force_appliers);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_remove(list_remove(scene->bodies, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  scene_add_bodies_force_creator(scene, forcer, aux, aux_get_bodies(aux, freer),
                                 freer);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer) {
  int forcer_already_exists = 0;
  for (size_t i = 0; i < list_size(scene->force_appliers); i++) {
    force_applier_t *applier =
        (force_applier_t *)list_get(scene->force_appliers, i);
    if (force_applier_get_force_creator(applier) == forcer) {
      force_applier_add_aux(applier, aux);
      forcer_already_exists = 1;
      break;
    }
  }
  if (!forcer_already_exists) {
    force_applier_t *new_applier = force_applier_init(forcer, freer);
    force_applier_add_aux(new_applier, aux);
    list_add(scene->force_appliers, new_applier);
  }
}

void scene_tick(scene_t *scene, double dt) {
  // run through every force_applier to apply forces and impulses
  for (size_t i = list_size(scene->force_appliers) - 1; i != -1; i--) {
    force_applier_t *applier =
        (force_applier_t *)list_get(scene->force_appliers, i);
    force_creator_t forcer = force_applier_get_force_creator(applier);
    for (size_t j = force_applier_auxes(applier) - 1; j != -1; j--) {
      void *aux = force_applier_get_aux(applier, j);
      list_t *bodies = aux_get_bodies(aux, force_applier_get_freer(applier));
      int removed = 0;
      for (size_t k = list_size(bodies) - 1; k != -1; k--) {
        body_t *body = (body_t *)list_get(bodies, k);
        if (body_is_removed(body)) {
          force_applier_remove_aux(applier, j);
          removed = 1;
          break;
        }
      }
      if (removed) {
        continue;
      }
      forcer(aux);
    }
  }

  // run through again to remove every aux with a body marked for removal
  for (size_t i = list_size(scene->force_appliers) - 1; i != -1; i--) {
    force_applier_t *applier =
        (force_applier_t *)list_get(scene->force_appliers, i);
    for (size_t j = force_applier_auxes(applier) - 1; j != -1; j--) {
      list_t *bodies = aux_get_bodies(force_applier_get_aux(applier, j),
                                      force_applier_get_freer(applier));
      for (size_t k = list_size(bodies) - 1; k != -1; k--) {
        body_t *body = (body_t *)list_get(bodies, k);
        if (body_is_removed(body)) {
          force_applier_remove_aux(applier, j);
          break;
        }
      }
    }
  }

  // removes from scene and frees all bodies marked for removal
  // and ticks all other bodies in scene
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = (body_t *)list_get(scene->bodies, i);
    if (body_is_removed(body)) {
      body_free((body_t *)list_remove(scene->bodies, i));
    } else {
      body_tick(body, dt);
    }
  }
}