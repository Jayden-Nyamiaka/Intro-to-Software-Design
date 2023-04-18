#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

/**
 * Holds all parameters needed to define, calculate, and store forces.
 * 
 */
typedef struct force_holder force_holder_t;

/**
 * Allocates memory for a force_holder.
 * 
 * @param forcer the function that will create forces
 * @param aux the aux holder that will be passed to the forcer whenever ru
 * @param freer the freer frunction that frees the aux
 * @return the new force_holder_t
 */
force_holder_t *force_holder_init(force_creator_t forcer, void *aux, free_func_t freer);

/**
 * Gets the forcer of the force_holder_t.
 *
 * @param holder a pointer to the force_holder
 * @return the force creator of the force holder
 */
force_creator_t force_holder_get_force_creator(force_holder_t *holder);

/**
 * Gets the aux of the force_holder_t.
 *
 * @param holder a pointer to the force_holder
 * @return the aux of the force holder
 */
void *force_holder_get_aux(force_holder_t *holder);

/**
 * Frees the force_holder_t.
 *
 * @param holder a pointer to the force_holder
 */
void force_holder_free(force_holder_t *holder);

/**
 * Adds a force creator to a scene that applies gravity the body and the floor.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 *
 * @param scene the scene containing the bodies
 * @param g the gravitational proportionality constant
 * @param body1 the body
 */
void create_earth_gravity(scene_t *scene, double g, body_t *body);

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
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

#endif // #ifndef __FORCES_H__
