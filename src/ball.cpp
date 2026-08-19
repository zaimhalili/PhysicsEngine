#include "../include/ball.hpp"

void UpdateVerlet(Ball &ball, float dt, float friction) {
  // Calculate implicit velocity
  Vector2D vel = Subtract(ball.position, ball.prevPosition);

  // Apply friction (damping)
  vel = Multiply(vel, friction);

  // Cap max velocity to prevent numerical explosion
  float maxSpeed = 15.0f;
  float currentSpeedSqr = LengthSqr(vel);
  if (currentSpeedSqr > maxSpeed * maxSpeed) {
    float speed = std::sqrt(currentSpeedSqr);
    vel = Multiply(vel, maxSpeed / speed);
  }

  // Save old position
  ball.prevPosition = ball.position;

  // Integrate position: pos + vel + (accel * dt^2)
  Vector2D accelTerm = Multiply(ball.acceleration, dt * dt);
  ball.position = Add(ball.position, Add(vel, accelTerm));

  // Reset force accumulation
  ball.acceleration = {0.0f, 0.0f};
}

void ConstrainVerlet(Ball &ball, float restitution, int screenWidth,
                     int screenHeight) {
  Vector2D vel = Subtract(ball.position, ball.prevPosition);
  float r = ball.radius;

  // X axis boundaries
  if (ball.position.x - r < 0.0f) {
    ball.position.x = r;
    ball.prevPosition.x = ball.position.x + (vel.x * restitution);
  } else if (ball.position.x + r > static_cast<float>(screenWidth)) {
    ball.position.x = static_cast<float>(screenWidth) - r;
    ball.prevPosition.x = ball.position.x + (vel.x * restitution);
  }

  // Y axis boundaries
  if (ball.position.y - r < 0.0f) {
    ball.position.y = r;
    ball.prevPosition.y = ball.position.y + (vel.y * restitution);
  } else if (ball.position.y + r > static_cast<float>(screenHeight)) {
    ball.position.y = static_cast<float>(screenHeight) - r;
    ball.prevPosition.y = ball.position.y + (vel.y * restitution);
  }
}