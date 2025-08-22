#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"

// The maximum number of times a node is allowed to be subdivided.
#define MAX_DEPTH 100

void quadtree_insert_recursive(QuadtreeNode *node, Body *body, int depth);
Quadrant get_quadrant(QuadtreeNode *node, Body *body);
void quadtree_subdivide(QuadtreeNode *node);


QuadtreeNode* quadtree_create(Vec2 center, double size) {
    QuadtreeNode *node = (QuadtreeNode *) malloc(sizeof(QuadtreeNode));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for quadtree node\n");
        return NULL;
    }
    // Initialize the node with the center and size
    node->center_region = center;
    node->size = size;
    node->body = NULL;
    node->total_mass = 0.0;
    node->center_of_mass = (Vec2){0.0, 0.0};

    for (int i = 0; i < 4; i++) {
        node->children[i] = NULL;
    }

    return node;
}

void quadtree_free(QuadtreeNode *node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        quadtree_free(node->children[i]);
    }
    free(node);
}


void quadtree_insert(QuadtreeNode *node, Body *body) {
    quadtree_insert_recursive(node, body, 0);
}

void quadtree_compute_mass_distribution(QuadtreeNode *node) {
    if (node == NULL) {
        return;
    }

    if (node->body != NULL) { // Base Case: Node is a leaf with a body
        node->total_mass = node->body->mass;
        node->center_of_mass = node->body->position;
    } else { // Recursive Case: Node is internal
        double total_mass = 0.0;
        Vec2 weighted_pos_sum = {0.0, 0.0};

        for (int i = 0; i < 4; i++) {
            if (node->children[i] != NULL) {
                quadtree_compute_mass_distribution(node->children[i]);
                total_mass += node->children[i]->total_mass;

                Vec2 child_com = node->children[i]->center_of_mass;
                double child_mass = node->children[i]->total_mass;
                weighted_pos_sum = add(weighted_pos_sum, scale(child_com, child_mass));
            }
        }
        node->total_mass = total_mass;

        if (total_mass > 0) {
            node->center_of_mass = scale(weighted_pos_sum, 1.0 / total_mass);
        } else {
            node->center_of_mass = (Vec2){0.0, 0.0};
        }
    }
}


void quadtree_insert_recursive(QuadtreeNode *node, Body *body, int depth) {
    // If node is an internal node (already subdivided), recurse.
    if (node->body == NULL && node->children[0] != NULL) {
        Quadrant quad = get_quadrant(node, body);
        quadtree_insert_recursive(node->children[quad], body, depth + 1);
        return;
    }

    if (node->body == NULL) {
        node->body = body;
        return;
    }

    // The node is an occupied leaf
    if (depth >= MAX_DEPTH) {
        node->body->mass += body->mass; 
        return;
    }

    // If we are not at max depth, subdivide and re-insert both bodies.
    quadtree_subdivide(node);
    
    // Hold onto the existing body before marking the node as internal.
    Body* existing_body = node->body;
    node->body = NULL; 

    // Re-insert the existing body into the correct new child quadrant.
    Quadrant old_body_quad = get_quadrant(node, existing_body);
    quadtree_insert_recursive(node->children[old_body_quad], existing_body, depth + 1);

    // Insert the new body into its correct child quadrant.
    Quadrant new_body_quad = get_quadrant(node, body);
    quadtree_insert_recursive(node->children[new_body_quad], body, depth + 1);
}

Quadrant get_quadrant(QuadtreeNode *node, Body *body) {
    if (body->position.y < node->center_region.y) {
        return (body->position.x < node->center_region.x) ? NORTHWEST : NORTHEAST;
    } else {
        return (body->position.x < node->center_region.x) ? SOUTHWEST : SOUTHEAST;
    }
}

void quadtree_subdivide(QuadtreeNode *node) {
    double child_size = node->size / 2.0;
    double offset = child_size / 2.0;

    Vec2 nw_center = {node->center_region.x - offset, node->center_region.y - offset};
    Vec2 ne_center = {node->center_region.x + offset, node->center_region.y - offset};
    Vec2 sw_center = {node->center_region.x - offset, node->center_region.y + offset};
    Vec2 se_center = {node->center_region.x + offset, node->center_region.y + offset};

    node->children[NORTHWEST] = quadtree_create(nw_center, child_size);
    node->children[NORTHEAST] = quadtree_create(ne_center, child_size);
    node->children[SOUTHWEST] = quadtree_create(sw_center, child_size);
    node->children[SOUTHEAST] = quadtree_create(se_center, child_size);

    for (int i = 0; i < 4; i++) {
        if (node->children[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for quadtree child node\n");
            for (int j = 0; j < i; j++) {
                quadtree_free(node->children[j]);
            }
            exit(EXIT_FAILURE);
        }
    }
}