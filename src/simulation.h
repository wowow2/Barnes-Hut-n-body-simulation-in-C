#ifndef SIMULATION_H
#define SIMULATION_H
#include "body.h"
#include <stdio.h>

typedef struct Simulation {
    Body* bodies;      // A dynamic array of all bodies in the simulation
    int num_bodies;   
    double theta;      
    double dt;         // The time step for the simulation (e.g., 0.01 seconds)
    double sim_size;   // The width of the square simulation area
    FILE* output_file;
    long long step_count;
} Simulation;

Simulation* simulation_create(Body* bodies, int num_bodies, double sim_size, double dt, double theta,const char* output_filename);
void simulation_free(Simulation* sim);
void simulation_step(Simulation* sim);
#endif