#include "vector.h"
#include <stdlib.h>

typedef struct vector {
    double x;
    double y;
} vector_t;

vector_t *vec_init(double x, double y) {
    vector_t *v = malloc(sizeof(vector_t));
    v->x = x;
    v->y = y;
    return v;
}

void vec_free(vector_t *vec) {
    free(vec);
}

vector_t *vec_add(vector_t *v1, vector_t *v2) {
    return vec_init(v1->x + v2->x, v1->y + v2->y);
}

double getX(vector_t *v) {
    return v->x;
}

double getY(vector_t *v) {
    return v->y;
}
