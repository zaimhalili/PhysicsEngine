#include "../include/ball.hpp"
#include "../include/physics.hpp"
#include "raylib.h"

void ShootBall(Ball &ball, Vector2D startPos, Vector2D endPos,
               float powerMultiplier) {
  Vector2D shotDir = Subtract(startPos, endPos);
  ball.prevPosition =
      Subtract(ball.position, Multiply(shotDir, powerMultiplier));
}

int main() {
  const int screenWidth = 1400;
  const int screenHeight = 1100;
  const float dt = 1.0f / 60.0f;
  const float friction = 0.99f;
  const float restitution = 0.8f;

  InitWindow(screenWidth, screenHeight, "Modular Physics Engine");
  SetTargetFPS(60);

  Ball whiteBall = {
      {100.0f, 300.0f}, {100.0f, 300.0f}, {0.0f, 0.0f}, 30.0f, WHITE};
  Ball blueBall = {
      {400.0f, 300.0f}, {400.0f, 300.0f}, {0.0f, 0.0f}, 30.0f, DARKBLUE};

  bool isDragging = false;
  Ball *selectedBall = nullptr;
  Vector2D dragStart = {0.0f, 0.0f};

  while (!WindowShouldClose()) {
    Vector2D mousePos = {GetMousePosition().x, GetMousePosition().y};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      // Check if mouse clicked inside redBall
      if (LengthSqr(Subtract(mousePos, whiteBall.position)) <=
          whiteBall.radius * whiteBall.radius) {
        selectedBall = &whiteBall;
        isDragging = true;
        dragStart = whiteBall.position;
      }
      // Check if mouse clicked inside blueBall
      else if (LengthSqr(Subtract(mousePos, blueBall.position)) <=
               blueBall.radius * blueBall.radius) {
        selectedBall = &blueBall;
        isDragging = true;
        dragStart = blueBall.position;
      }
    }

    // Release mouse button to strike the ball
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isDragging &&
        selectedBall != nullptr) {
      float powerMultiplier = 0.15f; // Adjust shot power responsiveness
      ShootBall(*selectedBall, dragStart, mousePos, powerMultiplier);

      isDragging = false;
      selectedBall = nullptr;
    }
    // Input (Smoother force)
    float force = 1000.0f;
    if (IsKeyDown(KEY_RIGHT))
      whiteBall.acceleration.x += force;
    if (IsKeyDown(KEY_LEFT))
      whiteBall.acceleration.x -= force;
    if (IsKeyDown(KEY_UP))
      whiteBall.acceleration.y -= force;
    if (IsKeyDown(KEY_DOWN))
      whiteBall.acceleration.y += force;

    // if (IsKeyDown(KEY_D))
    //   blueBall.acceleration.x += force;
    // if (IsKeyDown(KEY_A))
    //   blueBall.acceleration.x -= force;
    // if (IsKeyDown(KEY_W))
    //   blueBall.acceleration.y -= force;
    // if (IsKeyDown(KEY_S))
    //   blueBall.acceleration.y += force;

    // Integration
    UpdateVerlet(whiteBall, dt, friction);
    UpdateVerlet(blueBall, dt, friction);

    // Sub-stepping loop (Prevents sticking & overlap glitches)
    const int subSteps = 4;
    for (int i = 0; i < subSteps; ++i) {
      ResolveBallCollision(whiteBall, blueBall);
      ConstrainVerlet(whiteBall, restitution, screenWidth, screenHeight);
      ConstrainVerlet(blueBall, restitution, screenWidth, screenHeight);
    }

    // Drawing
    BeginDrawing();
    ClearBackground(DARKGREEN);
    if (isDragging && selectedBall != nullptr) {
      // Draw pull-back cue line (from ball to current mouse)
      DrawLineEx({whiteBall.position.x, whiteBall.position.y},
                 {mousePos.x, mousePos.y}, 5.0f, WHITE);

      // Draw trajectory guide line (points in shot direction)
      Vector2D shotDir = Subtract(dragStart, mousePos);
      Vector2D aimTarget = Add(selectedBall->position, shotDir);
      DrawLineEx({whiteBall.position.x, whiteBall.position.y},
                 {aimTarget.x, aimTarget.y}, 3.0f, LIME);
    }

    DrawCircleV({whiteBall.position.x, whiteBall.position.y}, whiteBall.radius,
                whiteBall.color);
    DrawCircleV({blueBall.position.x, blueBall.position.y}, blueBall.radius,
                blueBall.color);
    DrawFPS(10, screenHeight - 20);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}