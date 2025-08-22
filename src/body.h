#ifndef BODY_H
#define BODY_H

#include "vector.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    double mass;
} Body;

/*
applyForce: Applies a force vector to the body, updating its acceleration
update_body: Updates position and velocity based on time step
reset_acceleration: Resets the body's acceleration to zero
*/
void applyForce(Body *body, Vec2 force);
void update_body(Body *body, double dt);
void reset_acceleration(Body *body);

#endif
