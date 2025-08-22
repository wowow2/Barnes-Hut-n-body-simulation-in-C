#include <math.h>
#include "barnes_hut.h"
#include "vector.h"
#include <stdio.h>

// Define physical constants
const double EPSILON = 1e-9;   // factor to prevent division by zero
const double G = 6.67430e-11;

void barnes_hut_calculate_force(Body *target_body, QuadtreeNode *node, double theta) {
    if (node == NULL) {
        return;
    }

    // Node is an external node
    if (node->body != NULL) {
        if (node->body != target_body) {
            // Calculate force only if external node is not target, as target cannot exert force on itself
            Vec2 displacement = subtract(node->body->position, target_body->position);
            double distance = length(displacement);

            if (distance > 0) {
                // F = G * (m1 * m2) / (r^2 + ε)
                double force_magnitude = (G * target_body->mass * node->body->mass) / (distance * distance + EPSILON);
                // Create the force vector from force magnitude
                Vec2 force_vector = scale(displacement, force_magnitude / distance);
                applyForce(target_body, force_vector);
            }
        }
        return; // finished with external node
    }

    // Node is an internal node
    double s = node->size;
    double d = length(subtract(node->center_of_mass, target_body->position));

    if (d > 0 && (s / d) < theta) {
        // Treat the node as a single body
        Vec2 displacement = subtract(node->center_of_mass, target_body->position);
        double distance = length(displacement);

        if (distance > 0) {
            // F = G * (m1 * m2) / (r^2 + ε)
            double force_magnitude = (G * target_body->mass * node->total_mass) / (distance * distance + EPSILON);

            // Create the force vector from force magnitude
            Vec2 force_vector = scale(displacement, force_magnitude / distance);
            applyForce(target_body, force_vector);
            return;
    }
    else {
        // Cannot use the approximation, so recursively call function on children
        for (int i = 0; i < 4; i++) {
            barnes_hut_calculate_force(target_body, node->children[i], theta);
        }
    }
    }
}