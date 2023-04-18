#ifndef __FORCES_H__
#define __FORCES_H__

#include "collision.h"
#include "scene.h"

/**
 * Gets the list of bodies from the given aux
 *
 * @param aux a void pointer to the auxiliary variable
 * @return list of bodies stored within the aux
 */
list_t *aux_get_bodies(void *aux);

typedef struct force_applier force_applier_t;

force_applier_t *force_applier_init(force_creator_t forcer, free_func_t freer);

void force_applier_free(force_applier_t *applier);

size_t force_applier_auxes(force_applier_t *applier);

force_creator_t force_applier_get_force_creator(force_applier_t *applier);

void force_applier_add_aux(force_applier_t *applier, void *aux);

void *force_applier_get_aux(force_applier_t *applier, size_t index);

// remove and frees
void force_applier_remove_aux(force_applier_t *applier, size_t index);

/**
 * Adds a force creator to a scene that applies gravity the body and the floor.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 *
 * @param scene the scene containing the bodies
 * @param g the gravitational proportionality constant
 * @param bodies the list of bodies relating to the force (size = 1 for this
 * create)
 */
void create_earth_gravity(scene_t *scene, double g, list_t *bodies);

/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See
 * https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param bodies the list of bodies relating to the force (size = 2 for this
 * create)
 */
void create_newtonian_gravity(scene_t *scene, double G, list_t *bodies);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param bodies the list of bodies relating to the force (size = 2 for this
 * create)
 */
void create_spring(scene_t *scene, double k, list_t *bodies);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param bodies the list of bodies relating to the force (size = 1 for this
 * create)
 */
void create_drag(scene_t *scene, double gamma, list_t *bodies);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 *
 * @param scene the scene containing the bodies
 * @param bodies the list of bodies relating to the collision (size = 2 for this
 * create)
 */
void create_destructive_collision(scene_t *scene, list_t *bodies);

#endif // #ifndef __FORCES_H__
