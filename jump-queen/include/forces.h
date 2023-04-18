#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

/**
 * Gets the list of bodies from the given aux
 *
 * @param aux a void pointer to the auxiliary variable
 * @param freer function used to distinguish between different auxiliary structs
 * @return list of bodies stored within the aux
 */
list_t *aux_get_bodies(void *auxil, free_func_t freer);

typedef struct force_applier force_applier_t;

force_applier_t *force_applier_init(force_creator_t forcer, free_func_t freer);

void force_applier_free(force_applier_t *applier);

size_t force_applier_auxes(force_applier_t *applier);

force_creator_t force_applier_get_force_creator(force_applier_t *applier);

free_func_t force_applier_get_freer(force_applier_t *applier);

void force_applier_add_aux(force_applier_t *applier, void *aux);

void *force_applier_get_aux(force_applier_t *applier, size_t index);

// remove and frees
void force_applier_remove_aux(force_applier_t *applier, size_t index);

/**
 * A function called when a collision occurs.
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 */
typedef void (*collision_handler_t)(bool last_tick_collision, body_t *body1,
                                    body_t *body2, vector_t axis, void *aux);

/**
 * Collision handler to use for destructive collisions.
 */
void handle_destructive_collision(bool last_tick_collision, body_t *body1,
                                  body_t *body2, vector_t axis, void *aux);

/**
 * Collision handler to use for physics collisions.
 */
void handle_physics_collision(bool last_tick_collision, body_t *body1,
                              body_t *body2, vector_t axis, void *aux);
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
 * @param bodies bodies to apply the force to
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
 * @param bodies bodies to apply the force to
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
 * @param bodies bodies to apply the force to
 */
void create_drag(scene_t *scene, double gamma, list_t *bodies);

/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 *
 * @param scene the scene containing the bodies
 * @param bodies bodies to apply the collision to
 * @param handler a function to call whenever the bodies collide
 * @param aux an auxiliary value to pass to the handler
 * @param freer if non-NULL, a function to call in order to free aux
 */
void create_collision(scene_t *scene, list_t *bodies,
                      collision_handler_t handler, void *aux,
                      free_func_t freer);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * @param scene the scene containing the bodies
 * @param bodies bodies to apply the collision to
 */
void create_destructive_collision(scene_t *scene, list_t *bodies);

// auxiliary init for physics collisions
void *physics_collision_aux_init(double elasticity);

/**
 * Adds a force creator to a scene that applies impulses
 * to resolve collisions between two bodies in the scene.
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * You may remember from project01 that you should avoid applying impulses
 * multiple times while the bodies are still colliding.
 * You should also have a special case that allows either body1 or body2
 * to have mass INFINITY, as this is useful for simulating walls.
 *
 * @param scene the scene containing the bodies
 * @param elasticity the "coefficient of restitution" of the collision;
 * 0 is a perfectly inelastic collision and 1 is a perfectly elastic collision
 * @param bodies bodies to apply the collision to
 */
void create_physics_collision(scene_t *scene, double elasticity,
                              list_t *bodies);

#endif // #ifndef __FORCES_H__
