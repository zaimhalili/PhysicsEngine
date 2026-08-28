#ifndef POOL_GAME_HPP
#define POOL_GAME_HPP

#include "ball.hpp"
#include "grid.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <array>
#include <vector>

struct AimGuide {
  bool hasTarget = false;
  Vector2D direction = {0.0f, 0.0f};
  Vector2D targetDirection = {0.0f, 0.0f};
  Vector2D cueEnd = {0.0f, 0.0f};
  Vector2D targetEnd = {0.0f, 0.0f};
  int targetIndex = -1;
};

class TableRenderer {
public:
  explicit TableRenderer(Rectangle bounds);
  void Draw(const std::vector<Ball> &balls, const std::vector<int> &numbers,
            const std::array<Pocket, 6> &pockets, const AimGuide &guide,
            bool aiming, int pocketed, int shots, int player,
            const char *status) const;

private:
  Rectangle bounds;
  void DrawBall(const Ball &ball, int number) const;
};

class PoolGame {
public:
  explicit PoolGame(Rectangle table);
  void Update(float frameTime);
  void Draw() const;

private:
  Rectangle table;
  TableRenderer renderer;
  Grid grid;
  std::array<Pocket, 6> pockets;
  std::vector<Ball> balls;
  std::vector<int> numbers;
  std::vector<Ball *> activeBalls;
  std::vector<Ball *> candidates;
  AimGuide guide;
  Vector2D dragStart = {0.0f, 0.0f};
  bool aiming = false;
  bool shotInProgress = false;
  bool pocketedOwnBall = false;
  bool cueScratch = false;
  bool gameOver = false;
  bool eightBallWon = false;
  int pocketed = 0;
  int shots = 0;
  int currentPlayer = 0;
  std::array<int, 2> playerGroups = {0, 0};

  void Reset();
  void HandleInput();
  void Simulate(float frameTime);
  void ResolvePockets();
  bool AllStopped() const;
  void ShootCueBall(Vector2D mousePosition);
  AimGuide CalculateAimGuide(Vector2D mousePosition) const;
  const char *Status() const;
};

#endif
