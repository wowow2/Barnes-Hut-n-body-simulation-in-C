#include <stdlib.h>

#include "simulation.h"
#include "quadtree.h"
#include "barnes_hut.h"
#include "vector.h"

Simulation* simulation_create(Body* bodies, int num_bodies, double sim_size, double dt, double theta, 
    const char* output_filename) {
    Simulation* sim = (Simulation*)malloc(sizeof(Simulation));
    if (sim == NULL) {
        fprintf(stderr, "Failed to allocate memory for Simulation struct.\n");
        return NULL;
    }
    // Assign simulation parameters
    sim->bodies = bodies;
    sim->num_bodies = num_bodies;
    sim->theta = theta;
    sim->dt = dt;
    sim->sim_size = sim_size;

    // Assign state trackers
    sim->step_count = 0;
    sim->output_file = NULL;

    // Open the output file if a filename was provided
    if (output_filename != NULL) {
        sim->output_file = fopen(output_filename, "w");
        if (sim->output_file == NULL) {
            fprintf(stderr, "Failed to open output file: %s\n", output_filename);
        }
    }

    return sim;
}

void simulation_free(Simulation* sim) {
    if (sim == NULL) {
        return;
    }
    if (sim->output_file != NULL) {
        fclose(sim->output_file);
    }
    free(sim); // The caller will free the actual bodies
}

void simulation_step(Simulation* sim) {
    // Create the quadtree
    Vec2 center = {sim->sim_size / 2.0, sim->sim_size / 2.0};
    QuadtreeNode* root = quadtree_create(center, sim->sim_size);

    // Insert bodies into the quadtree
    for (int i = 0; i < sim->num_bodies; i++) {
        quadtree_insert(root, &sim->bodies[i]);
    }

    // Compute mass distribution
    quadtree_compute_mass_distribution(root);

    // Compute forces acting on each body
    for (int i = 0; i < sim->num_bodies; i++) {
        reset_acceleration(&sim->bodies[i]);
        barnes_hut_calculate_force(&sim->bodies[i], root, sim->theta);
    }

    // Update positions and velocities
    for (int i = 0; i < sim->num_bodies; i++) {
        update_body(&sim->bodies[i], sim->dt);
    }

    // Write to file
    if (sim->output_file != NULL) {
        for (int i = 0; i < sim->num_bodies; i++) {
            fprintf(sim->output_file, "%lld,%d,%f,%f,%f\n",
                    sim->step_count, i,
                    sim->bodies[i].position.x, sim->bodies[i].position.y, sim->bodies[i].mass);
        }
    }

    // Increment step count
    sim->step_count++;

    // Free the quadtree since we rebuild it every step
    quadtree_free(root);
}