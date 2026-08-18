#include "raylib.h"
#include "vector2.hpp"

void ConstrainAndBounce(Vector2D &pos, Vector2D &vel, float radius,
                        float restitution, int screenWidth, int screenHeight) {
  // Left border
  if (pos.x - radius < 0.0f) {
    pos.x = radius;
    vel.x = -vel.x * restitution;
  }
  // Right border
  if (pos.x + radius > screenWidth) {
    pos.x = screenWidth - radius;
    vel.x = -vel.x * restitution;
  }
  // Top border
  if (pos.y - radius < 0.0f) {
    pos.y = radius;
    vel.y = -vel.y * restitution;
  }
  // Bottom border
  if (pos.y + radius > screenHeight) {
    pos.y = screenHeight - radius;
    vel.y = -vel.y * restitution;
  }
}

int main() {
  const int screenWidth = 800;
  const int screenHeight = 600;
  const float radius1 = 40.0f;
  const float radius2 = 40.0f;
  const float speed = 2.0f;
  const float gravity = 1.0f;
  const float elasticity = 0.4f;
  InitWindow(screenWidth, screenHeight, "Physics Engine - Raylib Active");
  SetTargetFPS(60);

  Vector2D ballPos = {400.0f, 400.0f};
  Vector2D ball2Pos = {100.0f, 100.0f};

  Vector2D vel1 = {0.0f, 0.0f};
  Vector2D vel2 = {0.0f, 0.0f};

  while (!WindowShouldClose()) {
    

    // Ball 1 Movement (Arrow Keys)
    if (IsKeyDown(KEY_RIGHT))
      vel1.x += speed;
    if (IsKeyDown(KEY_LEFT))
      vel1.x -= speed;
    if (IsKeyDown(KEY_UP))
      vel1.y -= speed;
    if (IsKeyDown(KEY_DOWN))
      vel1.y += speed;

    // Ball 2 Movement (WASD Keys)
    if (IsKeyDown(KEY_D))
      vel2.x += speed;
    if (IsKeyDown(KEY_A))
      vel2.x -= speed;
    if (IsKeyDown(KEY_W))
      vel2.y -= speed;
    if (IsKeyDown(KEY_S))
      vel2.y += speed;

    vel1.y += gravity;
    vel2.y += gravity;

    ballPos = Add(ballPos, vel1);
    ball2Pos = Add(ball2Pos, vel2);

    Vector2D delta = Subtract(ballPos, ball2Pos);

    float distSqr = LengthSqr(delta);
    float radiiSum = radius1 + radius2;
    float radiiSumSqr = radiiSum * radiiSum;

    if (distSqr <= radiiSumSqr && distSqr > 0.0001f) {
      float distance = std::sqrt(distSqr);
      float overlap = radiiSum - distance;

      Vector2D normal = {delta.x / distance, delta.y / distance};
      Vector2D seperation = Multiply(normal, overlap * 0.5f);

      ballPos = Add(ballPos, seperation);
      ball2Pos = Subtract(ball2Pos, seperation);
    }

    ConstrainAndBounce(ballPos, vel1, radius1, elasticity, screenWidth,
                       screenHeight);
    ConstrainAndBounce(ball2Pos, vel2, radius2, elasticity, screenWidth,
                       screenHeight);

    BeginDrawing();
    ClearBackground(DARKGRAY);

    DrawText("My physics engine! Use Arrow Keys to Move", 10, 10, 20, RAYWHITE);
    DrawCircleV({ballPos.x, ballPos.y}, radius1, RED);
    DrawCircleV({ball2Pos.x, ball2Pos.y}, radius2, BLUE);

    DrawRectangle(0, -30, screenWidth, 30, BLACK);          // Top border
    DrawRectangle(0, screenHeight, screenWidth, 30, BLACK); // Bottom border
    DrawRectangle(-30, 0, 30, screenHeight, BLACK);         // Left border
    DrawRectangle(screenWidth, 0, 30, screenHeight, BLACK); // Right border

    DrawFPS(10, 570);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}