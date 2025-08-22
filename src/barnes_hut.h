#ifndef BARNES_HUT_H
#define BARNES_HUT_H

#include "body.h"
#include "quadtree.h"
/*
barnes_hut_calculate_force: Start from the root to calculate the total force exerted on the target_body by all
other bodies in the simulation. It uses the Barnes-Hut approximation criterion (s/d < theta) to decide whether
to treat a group of distant bodies as a single point mass.
*/
void barnes_hut_calculate_force(Body *target_body, QuadtreeNode *node, double theta);

#endif