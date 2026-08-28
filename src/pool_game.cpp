#include "../include/pool_game.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace {
constexpr float BallRadius = 13.0f;
constexpr float PocketRadius = 25.0f;
constexpr float MaxShotSpeed = 8.5f;
constexpr float AimLength = 460.0f;

Vector2D ToVector2D(Vector2 value) { return {value.x, value.y}; }
Vector2 ToVector2(Vector2D value) { return {value.x, value.y}; }

int BallGroup(int number) { return number == 8 ? 3 : number <= 7 ? 1 : 2; }
} // namespace

TableRenderer::TableRenderer(Rectangle bounds) : bounds(bounds) {}

void TableRenderer::DrawBall(const Ball &ball, int number) const {
  Vector2 center = ToVector2(ball.position);
  DrawCircleV(center, ball.radius + 1.5f, Fade(BLACK, 0.28f));
  DrawCircleV(center, ball.radius, ball.color);
  if (number > 0) {
    if (number > 8) {
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

void TableRenderer::Draw(const std::vector<Ball> &balls,
                         const std::vector<int> &numbers,
                         const std::array<Pocket, 6> &pockets,
                         const AimGuide &guide, bool aiming, int pocketed,
                         int shots, int player, const char *status) const {
  ClearBackground({18, 24, 34, 255});
  DrawRectangleRec({28.0f, 28.0f, 1144.0f, 744.0f}, Fade(BLACK, 0.35f));
  DrawRectangleRec(bounds, {38, 112, 78, 255});
  DrawRectangleLinesEx(bounds, 10.0f, {116, 73, 43, 255});
  DrawRectangleLinesEx({bounds.x + 12.0f, bounds.y + 12.0f,
                        bounds.width - 24.0f, bounds.height - 24.0f},
                       2.0f, {88, 168, 112, 255});

  for (const Pocket &pocket : pockets) {
    Vector2 center = ToVector2(pocket.position);
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
  DrawText(TextFormat("PLAYER %i", player + 1), 930, 92, 13,
           {216, 161, 82, 255});

  if (aiming) {
    DrawLineEx(ToVector2(balls.front().position), ToVector2(guide.cueEnd), 2.0f,
               Fade(RAYWHITE, 0.55f));
    if (guide.hasTarget) {
      DrawCircleV(ToVector2(guide.cueEnd), 3.5f, RAYWHITE);
      DrawLineEx(ToVector2(balls[guide.targetIndex].position),
                 ToVector2(guide.targetEnd), 2.0f, {216, 161, 82, 230});
    }
    Vector2 mouse = GetMousePosition();
    DrawLineEx(mouse, ToVector2(balls.front().position), 6.0f,
               {190, 143, 83, 255});
    DrawCircleV(mouse, 5.0f, RAYWHITE);
    float power = std::min(
        1.0f,
        Length(Subtract(balls.front().position, ToVector2D(mouse))) / 190.0f);
    DrawRectangle(72, 716, 240, 8, Fade(BLACK, 0.5f));
    DrawRectangle(72, 716, static_cast<int>(240.0f * power), 8,
                  {216, 161, 82, 255});
  }

  for (size_t index = 0; index < balls.size(); ++index) {
    if (balls[index].radius > 0.0f) {
      DrawBall(balls[index], numbers[index]);
    }
  }
  DrawText(status, 72, 742, 14, {209, 218, 207, 255});
}

PoolGame::PoolGame(Rectangle table)
    : table(table), renderer(table),
      grid(static_cast<int>(table.width), static_cast<int>(table.height), 32.0f,
           table.x, table.y),
      pockets({Pocket{{table.x + 24.0f, table.y + 24.0f}, PocketRadius},
               Pocket{{table.x + table.width * 0.5f, table.y + 18.0f},
                      PocketRadius},
               Pocket{{table.x + table.width - 24.0f, table.y + 24.0f},
                      PocketRadius},
               Pocket{{table.x + 24.0f, table.y + table.height - 24.0f},
                      PocketRadius},
               Pocket{{table.x + table.width * 0.5f,
                       table.y + table.height - 18.0f},
                      PocketRadius},
               Pocket{{table.x + table.width - 24.0f,
                       table.y + table.height - 24.0f},
                      PocketRadius}}) {
  Reset();
}

void PoolGame::Reset() {
  balls.clear();
  numbers.clear();
  const Vector2D cuePosition = {table.x + table.width * 0.25f,
                                table.y + table.height * 0.5f};
  balls.push_back({cuePosition, cuePosition, {0.0f, 0.0f}, BallRadius, WHITE});
  numbers.push_back(0);

  const Vector2D rackOrigin = {table.x + table.width * 0.72f,
                               table.y + table.height * 0.5f};
  const float spacing = BallRadius * 2.08f;
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
  activeBalls.reserve(16);
  candidates.reserve(16);
  pocketed = 0;
  shots = 0;
  currentPlayer = 0;
  playerGroups = {0, 0};
  aiming = false;
  shotInProgress = false;
  pocketedOwnBall = false;
  cueScratch = false;
  gameOver = false;
  eightBallWon = false;
}

bool PoolGame::AllStopped() const {
  for (const Ball &ball : balls) {
    if (ball.radius > 0.0f && !isBallStopped(ball)) {
      return false;
    }
  }
  return true;
}

void PoolGame::ShootCueBall(Vector2D mousePosition) {
  Vector2D shot = Subtract(dragStart, mousePosition);
  float distance = Length(shot);
  if (distance <= 8.0f) {
    return;
  }
  float speed = std::min(MaxShotSpeed, distance * 0.045f);
  balls.front().prevPosition =
      Subtract(balls.front().position, Multiply(shot, speed / distance));
  ++shots;
  shotInProgress = true;
  pocketedOwnBall = false;
  cueScratch = false;
}

AimGuide PoolGame::CalculateAimGuide(Vector2D mousePosition) const {
  AimGuide result;
  Vector2D shot = Subtract(dragStart, mousePosition);
  float shotLength = Length(shot);
  if (shotLength <= 0.0f) {
    return result;
  }
  result.direction = Multiply(shot, 1.0f / shotLength);
  result.cueEnd =
      Add(balls.front().position, Multiply(result.direction, AimLength));

  float nearestDistance = AimLength;
  for (size_t index = 1; index < balls.size(); ++index) {
    const Ball &ball = balls[index];
    if (ball.radius <= 0.0f) {
      continue;
    }
    Vector2D toBall = Subtract(ball.position, balls.front().position);
    float along = Dot(toBall, result.direction);
    if (along <= 0.0f) {
      continue;
    }
    float perpendicular =
        Length(Subtract(toBall, Multiply(result.direction, along)));
    float reach = balls.front().radius + ball.radius;
    if (perpendicular > reach) {
      continue;
    }
    float distance =
        along - std::sqrt(reach * reach - perpendicular * perpendicular);
    if (distance >= 0.0f && distance < nearestDistance) {
      nearestDistance = distance;
      result.hasTarget = true;
      Vector2D cueContact =
          Add(balls.front().position, Multiply(result.direction, distance));
      Vector2D collisionNormal = Subtract(ball.position, cueContact);
      float normalLength = Length(collisionNormal);
      if (normalLength <= 0.0f) {
        continue;
      }
      result.targetDirection = Multiply(collisionNormal, 1.0f / normalLength);
      result.cueEnd = Subtract(ball.position,
                               Multiply(result.targetDirection, ball.radius));
      result.targetEnd =
          Add(ball.position, Multiply(result.targetDirection, 130.0f));
      result.targetIndex = static_cast<int>(index);
    }
  }
  return result;
}

void PoolGame::HandleInput() {
  Vector2D mousePosition = ToVector2D(GetMousePosition());
  if (IsKeyPressed(KEY_R)) {
    Reset();
    return;
  }
  if (gameOver || !AllStopped()) {
    return;
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
      LengthSqr(Subtract(mousePosition, balls.front().position)) <=
          balls.front().radius * balls.front().radius) {
    aiming = true;
    dragStart = balls.front().position;
  }
  if (aiming) {
    guide = CalculateAimGuide(mousePosition);
  }
  if (aiming && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    ShootCueBall(mousePosition);
    aiming = false;
  }
}

void PoolGame::ResolvePockets() {
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
        int group = BallGroup(numbers[index]);
        if (group == 3) {
          eightBallWon = pocketed == 14 && playerGroups[currentPlayer] != 0;
          gameOver = true;
        } else {
          if (playerGroups[currentPlayer] == 0) {
            playerGroups[currentPlayer] = group;
            playerGroups[1 - currentPlayer] = 3 - group;
          }
          pocketedOwnBall =
              pocketedOwnBall || playerGroups[currentPlayer] == group;
        }
        ball.radius = 0.0f;
        ++pocketed;
      }
      break;
    }
  }
}

void PoolGame::Simulate(float frameTime) {
  const float dt = std::min(frameTime, 0.033f) / 8.0f;
  for (int step = 0; step < 8; ++step) {
    grid.Clear();
    activeBalls.clear();
    for (Ball &ball : balls) {
      if (ball.radius > 0.0f) {
        activeBalls.push_back(&ball);
        UpdateVerlet(ball, dt, 0.992f);
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
      ConstrainVerlet(*ball, 0.72f, table.x, table.y, table.x + table.width,
                      table.y + table.height);
    }
    ResolvePockets();
  }
}

const char *PoolGame::Status() const {
  if (gameOver) {
    return eightBallWon ? "PLAYER WINS  |  Press R for a new rack"
                        : "8-BALL FOUL  |  Press R for a new rack";
  }
  if (cueScratch) {
    return "SCRATCH  |  Turn passes to the other player";
  }
  if (aiming) {
    return "SET POWER  |  Release to shoot";
  }
  return "Click the cue ball, pull back, release to shoot";
}

void PoolGame::Update(float frameTime) {
  HandleInput();
  Simulate(frameTime);
  if (shotInProgress && AllStopped() && !aiming) {
    if (cueScratch || !pocketedOwnBall) {
      currentPlayer = 1 - currentPlayer;
    }
    shotInProgress = false;
  }
}

void PoolGame::Draw() const {
  renderer.Draw(balls, numbers, pockets, guide, aiming, pocketed, shots,
                currentPlayer, Status());
}
