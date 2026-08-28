#include "../include/pool_game.hpp"
#include "raylib.h"

int main() {
  InitWindow(1200, 800, "Pool Game made with a native physics engine");
  SetTargetFPS(60);

  PoolGame game({50.0f, 50.0f, 1100.0f, 700.0f});
  while (!WindowShouldClose()) {
    game.Update(GetFrameTime());
    BeginDrawing();
    game.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
