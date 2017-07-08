#include <stdlib.h>

#include "utils.h"

const int DIFFERENCE_MARGIN = 1000; // difference margin

int difference(__s32 x, __s32 y) {
    if (abs(x - y) > DIFFERENCE_MARGIN) {
        return 1;
    }
    return 0;
}

int coords_difference(struct coordinates* coords_first,
        struct coordinates* coords_second) {
    if (difference(*(coords_first->x_val), *(coords_second->x_val)) == 1
            && difference(*(coords_first->y_val), *(coords_second->y_val)) == 1
            && difference(*(coords_first->z_val), *(coords_second->z_val))
                    == 1) {
        return 1;
    }
    return 0;
}
