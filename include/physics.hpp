#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "ball.hpp"
#include "vector2.hpp"

struct Pocket {
  Vector2D position;
  float radius;
};

void ResolveBallCollision(Ball &b1, Ball &b2);

bool CheckPocketCollision(Ball &b1, const Pocket& pocket);

#endif // !PHYSICS_HPP
