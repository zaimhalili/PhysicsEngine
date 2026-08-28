#include "../include/ball.hpp"
#include "../include/grid.hpp"
#include "../include/physics.hpp"
#include "raylib.h"
#include <vector>

void ShootBall(Ball &ball, Vector2D startPos, Vector2D endPos,
               float powerMultiplier) {
  Vector2D shotDir = Subtract(startPos, endPos);
  ball.prevPosition =
      Subtract(ball.position, Multiply(shotDir, powerMultiplier));
}

int main() {
  const int screenWidth = 1200;
  const int screenHeight = 800;
  const float CELL_SIZE = 32.0f;
  const float friction = 0.99f;
  const float restitution = 0.6f;

  InitWindow(screenWidth, screenHeight, "Modular Physics Engine");
  SetTargetFPS(60);

  // Define table inside screen dimensions
  const Rectangle table = {50.0f, 50.0f, 1100.0f, 700.0f};

  // Instantiate spatial grid matching table bounds
  Grid grid(table.width, table.height, CELL_SIZE, table.x, table.y);

  std::vector<Ball *> candidates;
  candidates.reserve(16);

  Ball whiteBall = {
      {300.0f, 400.0f}, {300.0f, 400.0f}, {0.0f, 0.0f}, 15.0f, WHITE};
  Ball redBall = {{700.0f, 400.0f}, {700.0f, 400.0f}, {0.0f, 0.0f}, 15.0f, RED};

  // Top-left pocket placed inside the table boundary
  Pocket topLeftPocket = {{table.x + 25.0f, table.y + 25.0f}, 20.0f};

  bool isDragging = false;
  Ball *selectedBall = nullptr;
  Vector2D dragStart = {0.0f, 0.0f};
  bool redBallActive = true;

  while (!WindowShouldClose()) {
    bool cueBallStopped = isBallStopped(whiteBall);
    bool redBallStopped = !redBallActive || isBallStopped(redBall);

    Vector2D mousePos = {GetMousePosition().x, GetMousePosition().y};

    if (cueBallStopped && redBallStopped &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

    // Manual acceleration input for testing
    // float force = 200.0f;
    // if (IsKeyDown(KEY_RIGHT))
    //   whiteBall.acceleration.x += force;
    // if (IsKeyDown(KEY_LEFT))
    //   whiteBall.acceleration.x -= force;
    // if (IsKeyDown(KEY_UP))
    //   whiteBall.acceleration.y -= force;
    // if (IsKeyDown(KEY_DOWN))
    //   whiteBall.acceleration.y += force;

    // Spatial grid for better efficiency
    const int SUB_STEPS = 8;
    const float dt =
        (GetFrameTime() > 0.2f ? 0.016f : GetFrameTime()) / SUB_STEPS;

    for (int step = 0; step < SUB_STEPS; ++step) {

      grid.Clear();

      std::vector<Ball *> activeBalls;
      activeBalls.push_back(&whiteBall);
      if (redBallActive) {
        activeBalls.push_back(&redBall);
      }

      for (Ball *ball : activeBalls) {
        UpdateVerlet(*ball, dt, friction);
        grid.Insert(*ball);
      }

      for (Ball *ball : activeBalls) {
        grid.GetPossibleCollisions(*ball, candidates);

        for (Ball *neighbor : candidates) {
          // Avoid testing the same pair twice using address comparison
          if (neighbor > ball) {
            ResolveBallCollision(*ball, *neighbor);
          }
        }
      }

      for (Ball *ball : activeBalls) {
        ConstrainVerlet(*ball, restitution, table.x, table.y,
                        table.x + table.width, table.y + table.height);
      }

      // Pocket collisions
      if (redBallActive && CheckPocketCollision(redBall, topLeftPocket)) {
        redBallActive = false;
        if (selectedBall == &redBall) {
          isDragging = false;
          selectedBall = nullptr;
        }
      }

      if (CheckPocketCollision(whiteBall, topLeftPocket)) {
        whiteBall.position = {table.x + table.width / 2.0f,
                              table.y + table.height / 2.0f};
        whiteBall.prevPosition = whiteBall.position;
      }
    }

    BeginDrawing();
    ClearBackground(DARKBLUE);

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

    // Draw Aim Indicator
    if (cueBallStopped && redBallStopped && isDragging &&
        selectedBall != nullptr) {
      Vector2D ballPos = selectedBall->position;
      DrawLineEx({ballPos.x, ballPos.y}, {mousePos.x, mousePos.y}, 5.0f, WHITE);

      Vector2D shotDir = Subtract(dragStart, mousePos);
      Vector2D aimTarget = Add(ballPos, shotDir);
      DrawLineEx({ballPos.x, ballPos.y}, {aimTarget.x, aimTarget.y}, 3.0f,
                 LIGHTGRAY);
    }

    // Draw Active Balls
    DrawCircleV({whiteBall.position.x, whiteBall.position.y}, whiteBall.radius,
                whiteBall.color);

    if (redBallActive) {
      DrawCircleV({redBall.position.x, redBall.position.y}, redBall.radius,
                  redBall.color);
    }

    DrawFPS(10, screenHeight - 30);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}