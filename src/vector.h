#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    double x;
    double y;
} Vec2;

/*
Below functions define common vector operations needed
*/
Vec2 add(Vec2 a, Vec2 b);
Vec2 subtract(Vec2 a, Vec2 b);
Vec2 scale(Vec2 a, double scalar);
double dotProduct(Vec2 a, Vec2 b);
double length(Vec2 a);

#endif
