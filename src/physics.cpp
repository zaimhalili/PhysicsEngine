#include "../include/physics.hpp"
#include <cmath>

void ResolveBallCollision(Ball &b1, Ball &b2) {
  Vector2D delta = Subtract(b1.position, b2.position); // b2 -> b1
  float distSqr = LengthSqr(delta);
  float radiiSum = b1.radius + b2.radius;

  if (distSqr <= radiiSum * radiiSum && distSqr > 0.0001f) {
    float distance = std::sqrt(distSqr);
    float overlap = radiiSum - distance;
    Vector2D normal = {delta.x / distance, delta.y / distance};

    // 1. Positional separation
    Vector2D separation = Multiply(normal, overlap * 0.5f);
    b1.position = Add(b1.position, separation);
    b2.position = Subtract(b2.position, separation);
    b1.prevPosition = Add(b1.prevPosition, separation);
    b2.prevPosition = Subtract(b2.prevPosition, separation);

    // 2. Relative velocity check
    Vector2D v1 = Subtract(b1.position, b1.prevPosition);
    Vector2D v2 = Subtract(b2.position, b2.prevPosition);
    Vector2D relVel = Subtract(v1, v2);

    float velAlongNormal = Dot(relVel, normal);

    // Balls are approaching each other when velAlongNormal < 0
    if (velAlongNormal < 0.0f) {
      float restitution = 0.85f;
      float impulseMagnitude = -(1.0f + restitution) * velAlongNormal * 0.5f;
      Vector2D impulse = Multiply(normal, impulseMagnitude);

      // Apply impulse (b1 loses momentum along normal, b2 gains it)
      b1.prevPosition = Subtract(b1.prevPosition, impulse);
      b2.prevPosition = Add(b2.prevPosition, impulse);
    }
  }
}

bool CheckPocketCollision(Ball &b1, const Pocket &pocket) {
  Vector2D delta = Subtract(b1.position, pocket.position);

  float sinkDistance = pocket.radius * 0.7;
  return LengthSqr(delta) <= (sinkDistance * sinkDistance);
}