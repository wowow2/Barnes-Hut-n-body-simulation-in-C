#include "body.h"
#include <stddef.h>

void applyForce(Body *body, Vec2 force) {
    if (body == NULL || body->mass == 0.0) {
        return;
    }
    // F = m * a  =>  a = F / m
    Vec2 acceleration = scale(force, 1.0 / body->mass);
    body->acceleration = add(body->acceleration, acceleration);
}

void update_body(Body *body, double dt) {
    if (body == NULL || dt <= 0.0) {
        return;
    }
    // Update velocity: v_new = v_old + a * t
    body->velocity = add(body->velocity, scale(body->acceleration, dt));
    // Update position: p_new = p_old + v * t
    body->position = add(body->position, scale(body->velocity, dt));
}

void reset_acceleration(Body *body) {
    if (body == NULL) {
        return;
    }
    body->acceleration = (Vec2){0.0, 0.0};
}
