#include "vector.h"
#include <math.h>

Vec2 add(Vec2 a, Vec2 b) {
    Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

Vec2 subtract(Vec2 a, Vec2 b) {
    Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

Vec2 scale(Vec2 a, double scalar) {
    Vec2 result;
    result.x = a.x * scalar;
    result.y = a.y * scalar;
    return result;
}

double dotProduct(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

double length(Vec2 a) {
    return sqrt(dotProduct(a, a));
}
