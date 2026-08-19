#include "../include/physics.hpp"
#include <cmath>

void ResolveBallCollision(Ball &b1, Ball &b2) {
  Vector2D delta = Subtract(b1.position, b2.position);
  float distSqr = LengthSqr(delta);
  float radiiSum = b1.radius + b2.radius;

  if (distSqr <= radiiSum * radiiSum && distSqr > 0.0001f) {
    float distance = std::sqrt(distSqr);
    float overlap = radiiSum - distance;

    Vector2D normal = {delta.x / distance, delta.y / distance};
    Vector2D separation = Multiply(normal, overlap * 0.5f);

    // 1. Shift CURRENT position to stop overlap
    b1.position = Add(b1.position, separation);
    b2.position = Subtract(b2.position, separation);

    // 2. Shift PREVIOUS position by the SAME amount so implicit velocity stays
    // constant!
    b1.prevPosition = Add(b1.prevPosition, separation);
    b2.prevPosition = Subtract(b2.prevPosition, separation);
  }
}