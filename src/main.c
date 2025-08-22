#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "simulation.h"
#include "body.h"
#include "vector.h"

const double PI = 3.14159265358979323846;

// ============================================================================
//  Generate a single disc galaxy template
// ============================================================================
static Body* generate_galaxy_template(int num_bodies, double sim_size) {
    const double star_mass = 1.989e30;
    const double central_mass = star_mass * 1e6;
    const double disc_radius = sim_size / 4.0;

    Body* bodies = malloc(num_bodies * sizeof(Body));
    if (!bodies) return NULL;

    bodies[0] = (Body){ .mass = central_mass, .position = {0,0}, .velocity = {0,0}, .acceleration = {0,0} };

    for (int i = 1; i < num_bodies; i++) {
        bodies[i].mass = star_mass * (0.75 + 0.5 * ((double)rand() / RAND_MAX));
        double angle = ((double)rand() / RAND_MAX) * 2 * PI;
        double radius = sqrt((double)rand() / RAND_MAX) * disc_radius;
        bodies[i].position = (Vec2){ radius * cos(angle), radius * sin(angle) };
        if (radius > 0) {
            double orbital_speed = sqrt((6.67430e-11 * central_mass) / radius);
            bodies[i].velocity = (Vec2){ -orbital_speed * sin(angle), orbital_speed * cos(angle) };
        } else {
            bodies[i].velocity = (Vec2){0,0};
        }
        bodies[i].acceleration = (Vec2){0,0};
    }

    return bodies;
}

// ============================================================================
//  Create three-galaxy collision scenario
// ============================================================================
Body* create_three_galaxy_collision(int num_bodies, double sim_size) {
    Body* bodies = malloc(num_bodies * sizeof(Body));
    if (!bodies) return NULL;

    int n1 = num_bodies / 3;
    int n2 = num_bodies / 3;
    int n3 = num_bodies - n1 - n2;

    const double formation_radius = sim_size / 4.0;
    const double approach_speed   = 1.5e4;
    const double galaxy_size      = sim_size / 8.0;

    // Galaxy 1
    Body* g1 = generate_galaxy_template(n1, galaxy_size * 4.0);
    Vec2 center1 = {0.0, -formation_radius};
    Vec2 velocity1 = {0.0, approach_speed};
    for (int i = 0; i < n1; i++) {
        bodies[i].mass = g1[i].mass;
        bodies[i].position = add(g1[i].position, center1);
        bodies[i].velocity = add(g1[i].velocity, velocity1);
        bodies[i].acceleration = (Vec2){0,0};
    }
    free(g1);

    // Galaxy 2
    Body* g2 = generate_galaxy_template(n2, galaxy_size * 4.0);
    Vec2 center2 = {-formation_radius * cos(PI/6), formation_radius * sin(PI/6)};
    Vec2 velocity2 = {approach_speed * cos(PI/6), -approach_speed * sin(PI/6)};
    for (int i = 0; i < n2; i++) {
        bodies[n1 + i].mass = g2[i].mass;
        bodies[n1 + i].position = add(g2[i].position, center2);
        bodies[n1 + i].velocity = add(g2[i].velocity, velocity2);
        bodies[n1 + i].acceleration = (Vec2){0,0};
    }
    free(g2);

    // Galaxy 3
    Body* g3 = generate_galaxy_template(n3, galaxy_size * 4.0);
    Vec2 center3 = {formation_radius * cos(PI/6), formation_radius * sin(PI/6)};
    Vec2 velocity3 = {-approach_speed * cos(PI/6), -approach_speed * sin(PI/6)};
    for (int i = 0; i < n3; i++) {
        bodies[n1 + n2 + i].mass = g3[i].mass;
        bodies[n1 + n2 + i].position = add(g3[i].position, center3);
        bodies[n1 + n2 + i].velocity = add(g3[i].velocity, velocity3);
        bodies[n1 + n2 + i].acceleration = (Vec2){0,0};
    }
    free(g3);

    return bodies;
}

// ============================================================================
//  Main Simulation
// ============================================================================
int main(void) {
    const int num_bodies   = 2600;
    const double sim_size  = 1e19;
    const double dt        = 2e11;
    const double theta     = 0.2;
    const int steps        = 5000;
    const char* output_file = "output_3_galaxy_collision.csv";

    srand(time(NULL));

    Body* bodies = create_three_galaxy_collision(num_bodies, sim_size);
    if (!bodies) {
        fprintf(stderr, "Failed to create bodies.\n");
        return 1;
    }

    for (int i = 0; i < num_bodies; i++) {
        bodies[i].position.x += sim_size / 2.0;
        bodies[i].position.y += sim_size / 2.0;
    }

    Simulation* sim = simulation_create(bodies, num_bodies, sim_size, dt, theta, output_file);
    if (!sim) {
        free(bodies);
        return 1;
    }

    printf("Running 3-Galaxy Collision with %d bodies for %d steps...\n", num_bodies, steps);

    for (int i = 0; i < steps; i++) {
        simulation_step(sim);
        if (i % 50 == 0 || i == steps - 1)
            printf("Step %d/%d\n", i, steps);
    }

    printf("Simulation complete. Output: %s\n", output_file);

    simulation_free(sim);
    free(bodies);
    return 0;
}
