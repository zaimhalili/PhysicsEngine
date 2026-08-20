#ifndef BALL_HPP
#define BALL_HPP

#include "raylib.h"
#include "vector2.hpp"

struct Ball {
  Vector2D position;
  Vector2D prevPosition;
  Vector2D acceleration;
  float radius;
  Color color;
};

void UpdateVerlet(Ball& ball, float dt, float friction);
void ConstrainVerlet(Ball &ball, float restitution, float minX, float minY,
                     float maxX, float maxY);
bool isBallStopped(Ball& b1);

#endif // !BALL_HPP
