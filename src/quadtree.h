#ifndef QUADTREE_H
#define QUADTREE_H

#include "body.h"
#include "vector.h"

// Represents a quadrant in the 2D space
typedef enum Quadrant {
    NORTHWEST = 0,
    NORTHEAST = 1,
    SOUTHWEST = 2,
    SOUTHEAST = 3
} Quadrant;

typedef struct QuadtreeNode{
    Vec2 center_region; // Center of the node's region
    double size; 
    Body *body; // Pointer to the body contained in this node
    struct QuadtreeNode *children[4]; // Pointers to child nodes (quadrants)

    // Properties needed for Barnes Hut internal nodes
    double total_mass; // mass within the node and all its children
    Vec2 center_of_mass; // center of mass within the node and all its children
} QuadtreeNode;

/*
quadtree_create: Creates a new quadtree node by initalizing struct
quadtree_free: Recursively goes through tree and frees all allocated memory
get_quadrant: Determines the quadrant in which a body resides in
quadtree_subdivide: Subdivides a quadtree node into its four quadrants (four children)
quadtree_insert: Inserts a body into the quadtree
quadtree_compute_mass_distribution: Computes the center of mass and total mass for each node in the tree
*/
QuadtreeNode* quadtree_create(Vec2 center, double size);
void quadtree_free(QuadtreeNode *node);
Quadrant get_quadrant(QuadtreeNode *node, Body *body);
void quadtree_subdivide(QuadtreeNode *node);
void quadtree_insert(QuadtreeNode *node, Body *body);
void quadtree_compute_mass_distribution(QuadtreeNode *node);

#endif