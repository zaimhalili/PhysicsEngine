#include "../include/ball.hpp"
#include "../include/grid.hpp"
#include "../include/physics.hpp"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace {
constexpr int ScreenWidth = 1200;
constexpr int ScreenHeight = 800;
constexpr float BallRadius = 13.0f;
constexpr float PocketRadius = 25.0f;

void ShootBall(Ball &ball, Vector2D start, Vector2D end) {
  Vector2D shot = Subtract(start, end);
  float distance = Length(shot);
  if (distance <= 0.0f) {
    return;
  }
  float speed = std::min(8.5f, distance * 0.045f);
  ball.prevPosition = Subtract(ball.position, Multiply(shot, speed / distance));
}

void ResetGame(std::vector<Ball> &balls, std::vector<int> &numbers,
               const Rectangle &table) {
  balls.clear();
  numbers.clear();
  balls.reserve(16);
  numbers.reserve(16);
  Vector2D cuePosition = {table.x + table.width * 0.25f,
                          table.y + table.height * 0.5f};
  balls.push_back({cuePosition, cuePosition, {0.0f, 0.0f}, BallRadius, WHITE});
  numbers.push_back(0);

  Vector2D rackOrigin = {table.x + table.width * 0.72f,
                         table.y + table.height * 0.5f};
  const float spacing = BallRadius * 2.05f;
  const std::array<int, 15> rackNumbers = {1, 9,  2, 10, 8, 11, 3, 12,
                                           4, 13, 5, 14, 6, 15, 7};

  const std::array<Color, 15> colors = {
      YELLOW,   BLUE, RED,     PURPLE,  ORANGE,    GREEN,  MAROON, GOLD,
      DARKBLUE, LIME, MAGENTA, SKYBLUE, DARKGREEN, VIOLET, BROWN};
  for (int row = 0; row < 5; ++row) {
    for (int column = 0; column <= row; ++column) {
      int rackIndex = row * (row + 1) / 2 + column;
      Vector2D position = {rackOrigin.x + row * spacing,
                           rackOrigin.y + (column - row * 0.5f) * spacing};
      balls.push_back(
          {position,
           position,
           {0.0f, 0.0f},
           BallRadius,
           rackNumbers[rackIndex] == 8 ? BLACK : colors[rackIndex]});
      numbers.push_back(rackNumbers[rackIndex]);
    }
  }
}

bool AllStopped(const std::vector<Ball> &balls) {
  for (const Ball &ball : balls) {
    if (!isBallStopped(ball)) {
      return false;
    }
  }
  return true;
}

void DrawBall(const Ball &ball, int number) {
  Vector2 center = {ball.position.x, ball.position.y};
  DrawCircleV(center, ball.radius + 1.5f, Fade(BLACK, 0.28f));
  DrawCircleV(center, ball.radius, ball.color);
  if (number > 0) {
    if (number >= 9 && number != 8) {
      DrawRectangle(static_cast<int>(center.x - ball.radius),
                    static_cast<int>(center.y - ball.radius * 0.32f),
                    static_cast<int>(ball.radius * 2.0f),
                    static_cast<int>(ball.radius * 0.64f), WHITE);
    }
    DrawCircleV(center, ball.radius * 0.45f, RAYWHITE);
    DrawText(std::to_string(number).c_str(), static_cast<int>(center.x) - 3,
             static_cast<int>(center.y) - 5, 10, BLACK);
  }
  DrawCircleGradient({center.x - 4.0f, center.y - 5.0f}, 4.0f, WHITE,
                     Fade(WHITE, 0.0f));
}
} // namespace

int main() {
  const Rectangle table = {50.0f, 50.0f, 1100.0f, 700.0f};
  const float friction = 0.992f;
  const float restitution = 0.72f;
  const std::array<Pocket, 6> pockets = {
      Pocket{{table.x + 24.0f, table.y + 24.0f}, PocketRadius},
      Pocket{{table.x + table.width * 0.5f, table.y + 18.0f}, PocketRadius},
      Pocket{{table.x + table.width - 24.0f, table.y + 24.0f}, PocketRadius},
      Pocket{{table.x + 24.0f, table.y + table.height - 24.0f}, PocketRadius},
      Pocket{{table.x + table.width * 0.5f, table.y + table.height - 18.0f},
             PocketRadius},
      Pocket{{table.x + table.width - 24.0f, table.y + table.height - 24.0f},
             PocketRadius}};

  InitWindow(ScreenWidth, ScreenHeight, "CMU Pool Lab");
  SetTargetFPS(60);
  Grid grid(static_cast<int>(table.width), static_cast<int>(table.height),
            32.0f, table.x, table.y);
  std::vector<Ball> balls;
  std::vector<int> numbers;
  std::vector<Ball *> activeBalls;
  std::vector<Ball *> candidates;
  ResetGame(balls, numbers, table);

  bool aiming = false;
  Vector2D dragStart = {0.0f, 0.0f};
  int pocketed = 0;
  int shots = 0;
  int currentPlayer = 0;
  std::array<int, 2> playerGroups = {0, 0};
  bool shotInProgress = false;
  bool pocketedOwnBall = false;
  bool cueScratch = false;
  bool gameOver = false;
  bool eightBallWon = false;

  while (!WindowShouldClose()) {
    Vector2 mouse = GetMousePosition();
    Vector2D mousePos = {mouse.x, mouse.y};
    bool stopped = AllStopped(balls);
    Ball &cueBall = balls.front();

    if (IsKeyPressed(KEY_R)) {
      ResetGame(balls, numbers, table);
      pocketed = 0;
      shots = 0;
      currentPlayer = 0;
      playerGroups = {0, 0};
      shotInProgress = false;
      pocketedOwnBall = false;
      cueScratch = false;
      gameOver = false;
      eightBallWon = false;
      aiming = false;
    }

    if (stopped && !gameOver && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        LengthSqr(Subtract(mousePos, cueBall.position)) <=
            cueBall.radius * cueBall.radius) {
      aiming = true;
      dragStart = cueBall.position;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && aiming) {
      float shotLength = Length(Subtract(dragStart, mousePos));
      if (shotLength > 8.0f) {
        ShootBall(cueBall, dragStart, mousePos);
        ++shots;
        shotInProgress = true;
        pocketedOwnBall = false;
        cueScratch = false;
      }
      aiming = false;
    }

    float frameTime = std::min(GetFrameTime(), 0.033f);
    const float dt = frameTime / 8.0f;
    for (int step = 0; step < 8; ++step) {
      grid.Clear();
      activeBalls.clear();
      for (Ball &ball : balls) {
        if (ball.radius > 0.0f) {
          activeBalls.push_back(&ball);
          UpdateVerlet(ball, dt, friction);
          grid.Insert(ball);
        }
      }

      for (Ball *ball : activeBalls) {
        grid.GetPossibleCollisions(*ball, candidates);
        for (Ball *neighbor : candidates) {
          if (neighbor > ball) {
            ResolveBallCollision(*ball, *neighbor);
          }
        }
      }

      for (Ball *ball : activeBalls) {
        ConstrainVerlet(*ball, restitution, table.x, table.y,
                        table.x + table.width, table.y + table.height);
      }

      for (size_t index = 0; index < balls.size(); ++index) {
        Ball &ball = balls[index];
        if (ball.radius <= 0.0f) {
          continue;
        }
        for (const Pocket &pocket : pockets) {
          if (!CheckPocketCollision(ball, pocket)) {
            continue;
          }
          if (index == 0) {
            cueScratch = true;
            ball.position = {table.x + table.width * 0.25f,
                             table.y + table.height * 0.5f};
            ball.prevPosition = ball.position;
          } else {
            int ballGroup = numbers[index] == 8 ? 3 : numbers[index] <= 7 ? 1 : 2;
            if (ballGroup == 3) {
              eightBallWon = pocketed == 14;
              gameOver = true;
            } else {
              if (playerGroups[currentPlayer] == 0) {
                playerGroups[currentPlayer] = ballGroup;
                playerGroups[1 - currentPlayer] = 3 - ballGroup;
              }
              if (playerGroups[currentPlayer] == ballGroup) {
                pocketedOwnBall = true;
              }
            }
            ball.radius = 0.0f;
            ++pocketed;
          }
          break;
        }
      }
    }

    if (shotInProgress && stopped && !aiming) {
      if (cueScratch || !pocketedOwnBall) {
        currentPlayer = 1 - currentPlayer;
      }
      shotInProgress = false;
    }

    BeginDrawing();
    ClearBackground({18, 24, 34, 255});
    DrawRectangleRec({28.0f, 28.0f, 1144.0f, 744.0f}, Fade(BLACK, 0.35f));
    DrawRectangleRec(table, {38, 112, 78, 255});
    DrawRectangleLinesEx(table, 10.0f, {116, 73, 43, 255});
    DrawRectangleLinesEx({table.x + 12.0f, table.y + 12.0f, table.width - 24.0f,
                          table.height - 24.0f},
                         2.0f, {88, 168, 112, 255});

    for (const Pocket &pocket : pockets) {
      Vector2 center = {pocket.position.x, pocket.position.y};
      DrawCircleV(center, pocket.radius + 8.0f, {25, 30, 31, 255});
      DrawCircleV(center, pocket.radius, BLACK);
      DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y),
                      pocket.radius + 2.0f, {105, 68, 42, 255});
    }

    DrawText("Pool Game", 72, 66, 24, RAYWHITE);
    DrawText("PHYSICS PLAYGROUND", 75, 92, 11, {164, 199, 173, 255});
    DrawText(TextFormat("BALLS  %02i", pocketed), 910, 68, 16, RAYWHITE);
    DrawText(TextFormat("SHOTS  %02i", shots), 1010, 68, 16,
             {216, 224, 210, 255});
    DrawText(TextFormat("PLAYER %i", currentPlayer + 1), 930, 92, 13,
         {216, 161, 82, 255});

    if (aiming) {
      Vector2D shot = Subtract(dragStart, mousePos);
      float length = std::min(260.0f, Length(shot));
      Vector2D direction = length > 0.0f ? Multiply(shot, 1.0f / Length(shot))
                                         : Vector2D{0.0f, 0.0f};
      Vector2D guideEnd = Add(cueBall.position, Multiply(direction, 420.0f));
      DrawLineEx({cueBall.position.x, cueBall.position.y},
                 {guideEnd.x, guideEnd.y}, 2.0f, Fade(RAYWHITE, 0.45f));
      DrawLineEx({mouse.x, mouse.y}, {cueBall.position.x, cueBall.position.y},
                 6.0f, {190, 143, 83, 255});
      DrawCircleV({mouse.x, mouse.y}, 5.0f, RAYWHITE);
      DrawRectangle(72, 716, 240, 8, Fade(BLACK, 0.5f));
      DrawRectangle(72, 716, static_cast<int>(240.0f * length / 260.0f), 8,
                    {216, 161, 82, 255});
    }

    for (size_t index = 0; index < balls.size(); ++index) {
      if (balls[index].radius > 0.0f) {
        DrawBall(balls[index], index == 0 ? 0 : static_cast<int>(index));
      }
    }

    const char *status =
      gameOver         ? (eightBallWon ? "PLAYER WINS  |  Press R for a new rack"
               : "8-BALL FOUL  |  Press R for a new rack")
      : cueScratch      ? "SCRATCH  |  Turn passes to the other player"
      : aiming          ? "SET POWER  |  Release to shoot"
               : "Click the cue ball, pull back, release to shoot";
    DrawText(status, 72, 742, 14, {209, 218, 207, 255});
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
