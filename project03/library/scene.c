#include "scene.h"
#include "assert.h"
#include "body.h"

const size_t INTIAL_CAPACITY = 5;

typedef struct scene {
  list_t *bodies;
  size_t size;
} scene_t;

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  scene->bodies = list_init(INTIAL_CAPACITY, (free_func_t)body_free);
  scene->size = 0;
  assert(scene->bodies != NULL);
  return scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return scene->size; }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
  scene->size++;
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_free(list_remove(scene->bodies, index));
  scene->size--;
}

void scene_tick(scene_t *scene, double dt) {
  for (int i = 0; i < scene->size; i++) {
    body_tick(list_get(scene->bodies, i), dt);
  }
}