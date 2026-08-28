#include "../include/ball.hpp"

void UpdateVerlet(Ball &ball, float dt, float friction) {
  // Calculate implicit velocity
  Vector2D vel = Subtract(ball.position, ball.prevPosition);

  // Apply friction (damping)
  vel = Multiply(vel, friction);

  // Stop micro drifts if the velocity reaches the threshold
  if (LengthSqr(vel) < 0.0025f) { // threshold * threshold
    vel = {0.0f, 0.0f};
  }

  // Save old position
  ball.prevPosition = ball.position;

  // Integrate position: pos + vel + (accel * dt^2)
  Vector2D accelTerm = Multiply(ball.acceleration, dt * dt);
  ball.position = Add(ball.position, Add(vel, accelTerm));

  // Reset force accumulation
  ball.acceleration = {0.0f, 0.0f};
}

void ConstrainVerlet(Ball &ball, float restitution, float minX, float minY,
                     float maxX, float maxY) {
  Vector2D vel = Subtract(ball.position, ball.prevPosition);
  float r = ball.radius;

  // X axis boundaries (Table left & right)
  if (ball.position.x - r < minX) {
    ball.position.x = minX + r;
    ball.prevPosition.x = ball.position.x + (vel.x * restitution);
  } else if (ball.position.x + r > maxX) {
    ball.position.x = maxX - r;
    ball.prevPosition.x = ball.position.x + (vel.x * restitution);
  }

  // Y axis boundaries (Table top & bottom)
  if (ball.position.y - r < minY) {
    ball.position.y = minY + r;
    ball.prevPosition.y = ball.position.y + (vel.y * restitution);
  } else if (ball.position.y + r > maxY) {
    ball.position.y = maxY - r;
    ball.prevPosition.y = ball.position.y + (vel.y * restitution);
  }
}

bool isBallStopped(const Ball &ball, float threshold) {
  Vector2D vel = Subtract(ball.position, ball.prevPosition);

  return LengthSqr(vel) < (threshold * threshold);
}