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
    // 1. Input (Smoother force)
    float force = 1000.0f;
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

    // 2. Integration
    UpdateVerlet(redBall, dt, friction);
    UpdateVerlet(blueBall, dt, friction);

    // 3. Sub-stepping loop (Prevents sticking & overlap glitches)
    const int subSteps = 4;
    for (int i = 0; i < subSteps; ++i) {
      ResolveBallCollision(redBall, blueBall);
      ConstrainVerlet(redBall, restitution, screenWidth, screenHeight);
      ConstrainVerlet(blueBall, restitution, screenWidth, screenHeight);
    }

    // 4. Drawing
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