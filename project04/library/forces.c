#include "forces.h"
#include <stdlib.h>

const double BLOW_UP_DISTANCE = 5; // 5 for tests | 8 for preference

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

void auxiliary_free(auxiliary_t *aux) {
    list_free(aux->bodies);
    free(aux);
}


typedef struct force_holder {
    force_creator_t forcer;
    auxiliary_t *aux;
    free_func_t freer;
} force_holder_t;

force_holder_t *force_holder_init(force_creator_t forcer, void *aux, free_func_t freer) {
    force_holder_t *holder = malloc(sizeof(force_holder_t));
    holder->forcer = forcer;
    holder->aux = (auxiliary_t *)aux;
    holder->freer = freer;
    return holder;
}


force_creator_t force_holder_get_force_creator(force_holder_t *holder) { return holder->forcer;  }

void *force_holder_get_aux(force_holder_t *holder) { return holder->aux;  }

void force_holder_free(force_holder_t *holder) {
    holder->freer( (auxiliary_t *) (holder->aux) );
    free(holder);
}

void apply_earth_gravity(void *aux) {
    auxiliary_t *auxil = (auxiliary_t *)aux;
    double g = auxil->constant;
    double mass = (double) body_get_mass(list_get(auxil->bodies, 0));
    vector_t force = {0, -1 * mass * g };
    body_add_force(list_get(auxil->bodies, 0), force);
}

void create_earth_gravity(scene_t *scene, double g, body_t *body) {
    list_t *bodies = list_init(1, NULL);
    list_add(bodies, body);
    auxiliary_t *aux = auxiliary_init(g, bodies);
    scene_add_force_creator(scene, apply_earth_gravity, aux, (free_func_t) auxiliary_free);
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
    double mass_1 = (double) body_get_mass(body_1);
    double mass_2 = (double) body_get_mass(body_2);
    double force_mag = G * mass_1 * mass_2 / (distance*distance);
    vector_t unit_vec = vec_multiply(1.0/distance, vec_subtract(centroid_1, centroid_2));
    vector_t force = vec_multiply(force_mag, unit_vec);
    body_add_force(body_1, vec_negate(force));
    body_add_force(body_2, force);
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2) {
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    auxiliary_t *aux = auxiliary_init(G, bodies);
    scene_add_force_creator(scene, apply_newtonian_gravity, aux, (free_func_t) auxiliary_free);
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

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2){
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    auxiliary_t *aux = auxiliary_init(k, bodies);
    scene_add_force_creator(scene, apply_spring, aux, (free_func_t) auxiliary_free);
}

void apply_drag(void *aux) {
    auxiliary_t *auxil = (auxiliary_t *)aux;
    body_t *body = (body_t *)list_get(auxil->bodies, 0);
    double gamma = auxil->constant;
    vector_t velocity = body_get_velocity(body);
    vector_t force = vec_multiply(-1 * gamma, velocity);
    body_add_force(body, force);
}

void create_drag(scene_t *scene, double gamma, body_t *body){
    list_t *bodies = list_init(1, NULL);
    list_add(bodies, body);
    auxiliary_t *aux = auxiliary_init(gamma, bodies);
    scene_add_force_creator(scene, apply_drag, aux, (free_func_t) auxiliary_free);
}