#include "../include/ball.hpp"
#include "../include/physics.hpp"
#include "raylib.h"


int main() {
  const int screenWidth = 800;
  const int screenHeight = 600;
  const float dt = 1.0f / 60.0f;
  const float friction = 0.99f;
  const float restitution = 0.8f;

  InitWindow(screenWidth, screenHeight, "Modular Physics Engine");
  SetTargetFPS(60);

  Ball redBall = {{100.0f, 300.0f}, {100.0f, 300.0f}, {0.0f, 0.0f}, 30.0f, RED};
  Ball blueBall = {
      {400.0f, 300.0f}, {400.0f, 300.0f}, {0.0f, 0.0f}, 30.0f, BLUE};

  while (!WindowShouldClose()) {
    // Input
    float force = 250.0f;
    if (IsKeyDown(KEY_RIGHT))
      redBall.acceleration.x += force;
    if (IsKeyDown(KEY_LEFT))
      redBall.acceleration.x -= force;
    if (IsKeyDown(KEY_UP))
      redBall.acceleration.y -= force;
    if (IsKeyDown(KEY_DOWN))
      redBall.acceleration.y += force;

    if (IsKeyDown(KEY_D))
      blueBall.acceleration.x += force;
    if (IsKeyDown(KEY_A))
      blueBall.acceleration.x -= force;
    if (IsKeyDown(KEY_W))
      blueBall.acceleration.y -= force;
    if (IsKeyDown(KEY_S))
      blueBall.acceleration.y += force;

    // Physics Updates
    UpdateVerlet(redBall, dt, friction);
    UpdateVerlet(blueBall, dt, friction);

    // Collisions
    ResolveBallCollision(redBall, blueBall);

    // Constraints
    ConstrainVerlet(redBall, restitution, screenWidth, screenHeight);
    ConstrainVerlet(blueBall, restitution, screenWidth, screenHeight);

    // Drawing
    BeginDrawing();
    ClearBackground(DARKGRAY);
    DrawCircleV({redBall.position.x, redBall.position.y}, redBall.radius,
                redBall.color);
    DrawCircleV({blueBall.position.x, blueBall.position.y}, blueBall.radius,
                blueBall.color);
    DrawFPS(10, 570);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}