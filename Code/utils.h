#ifndef _utils_h_
#define _utils_h_

const int DIFFERENCE_MARGIN = 1000; // difference margin

int difference(__s32 x, __s32 y); 
int coords_difference(struct coordinates* coords_first, struct coordinates* coords_second); 

#endif
