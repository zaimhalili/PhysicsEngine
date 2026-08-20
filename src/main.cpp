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
  const float restitution = 0.6f;

  InitWindow(screenWidth, screenHeight, "Modular Physics Engine");
  SetTargetFPS(60);

  const Rectangle table = {100.0f, 100.0f, 1200.0f, 900.0f};

  Ball whiteBall = {
      {300.0f, 550.0f}, {300.0f, 550.0f}, {0.0f, 0.0f}, 15.0f, WHITE};
  Ball redBall = {
      {700.0f, 550.0f}, {700.0f, 550.0f}, {0.0f, 0.0f}, 15.0f, RED};

  // Top-left pocket placed inside the table boundary
  Pocket topLeftPocket = {{table.x + 25.0f, table.y + 25.0f}, 20.0f};

  bool isDragging = false;
  Ball *selectedBall = nullptr;
  Vector2D dragStart = {0.0f, 0.0f};
  bool redBallActive = true;

  while (!WindowShouldClose()) {
    Vector2D mousePos = {GetMousePosition().x, GetMousePosition().y};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (LengthSqr(Subtract(mousePos, whiteBall.position)) <=
          whiteBall.radius * whiteBall.radius) {
        selectedBall = &whiteBall;
        isDragging = true;
        dragStart = whiteBall.position;
      } else if (redBallActive &&
                 LengthSqr(Subtract(mousePos, redBall.position)) <=
                     redBall.radius * redBall.radius) {
        selectedBall = &redBall;
        isDragging = true;
        dragStart = redBall.position;
      }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isDragging &&
        selectedBall != nullptr) {
      float powerMultiplier = 0.06f;
      ShootBall(*selectedBall, dragStart, mousePos, powerMultiplier);

      isDragging = false;
      selectedBall = nullptr;
    }

    float force = 200.0f;
    if (IsKeyDown(KEY_RIGHT))
      whiteBall.acceleration.x += force;
    if (IsKeyDown(KEY_LEFT))
      whiteBall.acceleration.x -= force;
    if (IsKeyDown(KEY_UP))
      whiteBall.acceleration.y -= force;
    if (IsKeyDown(KEY_DOWN))
      whiteBall.acceleration.y += force;

    UpdateVerlet(whiteBall, dt, friction);
    if (redBallActive) {
      UpdateVerlet(redBall, dt, friction);
    }

    const int subSteps = 4;
    for (int i = 0; i < subSteps; ++i) {
      if (redBallActive) {
        ResolveBallCollision(whiteBall, redBall);
        ConstrainVerlet(redBall, restitution, table.x, table.y,
                        table.x + table.width, table.y + table.height);

        // Keep inside table bounds
        if (redBall.position.x - redBall.radius < table.x) {
          redBall.position.x = table.x + redBall.radius;
        }
        if (redBall.position.y - redBall.radius < table.y) {
          redBall.position.y = table.y + redBall.radius;
        }

        if (CheckPocketCollision(redBall, topLeftPocket)) {
          redBallActive = false;
          if (selectedBall == &redBall) {
            isDragging = false;
            selectedBall = nullptr;
          }
        }
      }

      if (CheckPocketCollision(whiteBall, topLeftPocket)) {
        whiteBall.position = {table.x + table.width / 2.0f,
                              table.y + table.height / 2.0f};
        whiteBall.prevPosition = whiteBall.position;
      }

      ConstrainVerlet(whiteBall, restitution, table.x, table.y,
                      table.x + table.width, table.y + table.height);

      // Keep inside table bounds
      if (whiteBall.position.x - whiteBall.radius < table.x) {
        whiteBall.position.x = table.x + whiteBall.radius;
      }
      if (whiteBall.position.y - whiteBall.radius < table.y) {
        whiteBall.position.y = table.y + whiteBall.radius;
      }
    }

    BeginDrawing();
    ClearBackground(DARKBLUE); // Wooden frame around table

    // Draw Table Felt Area
    DrawRectangleRec(table, SKYBLUE);

    // Draw Pocket
    DrawCircleV({topLeftPocket.position.x, topLeftPocket.position.y},
                topLeftPocket.radius + 6.0f, DARKGRAY);
    DrawCircleV({topLeftPocket.position.x, topLeftPocket.position.y},
                topLeftPocket.radius, BLACK);
    DrawCircleLines(static_cast<int>(topLeftPocket.position.x),
                    static_cast<int>(topLeftPocket.position.y),
                    topLeftPocket.radius + 6.0f, GRAY);

    // Aim Trajectory
    if (isDragging && selectedBall != nullptr && whiteBall.acceleration.x == 0.0f && whiteBall.acceleration.y == 0.0f) {
      Vector2D ballPos = selectedBall->position;
      DrawLineEx({ballPos.x, ballPos.y}, {mousePos.x, mousePos.y}, 5.0f, WHITE);

      Vector2D shotDir = Subtract(dragStart, mousePos);
      Vector2D aimTarget = Add(ballPos, shotDir);
      DrawLineEx({ballPos.x, ballPos.y}, {aimTarget.x, aimTarget.y}, 3.0f,
                 LIGHTGRAY);
    }

    // Draw Balls
    DrawCircleV({whiteBall.position.x, whiteBall.position.y}, whiteBall.radius,
                whiteBall.color);

    if (redBallActive) {
      DrawCircleV({redBall.position.x, redBall.position.y}, redBall.radius,
                  redBall.color);
    }

    DrawFPS(10, screenHeight - 20);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}