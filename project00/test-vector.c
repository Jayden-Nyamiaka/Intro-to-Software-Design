#include <stdio.h>
#include <assert.h>
#include "vector.c"

int main(int argc, char *argv[]) {
    double x1 = 5.0, y1 = 6.0;
    vector_t *v1 = vec_init(x1, y1);
    double x2 = -4.0, y2 = 3.0;
    vector_t *v2 = vec_init(x2, y2);
    vector_t *v3 = vec_add(v1, v2);

    assert(x1 == getX(v1) && y1 == getY(v1));
    assert(x2 == getX(v2) && y2 == getY(v2));
    assert(getX(v3) == getX(v1) + getX(v2) && getY(v3) == getY(v1) + getY(v2));

    vec_free(v1);
    vec_free(v2);
    vec_free(v3);

    printf("ALL TESTS PASSED\n");
    return 0;
}