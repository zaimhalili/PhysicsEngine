#include <iostream>
#include "raylib.h"
#include "vector2.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float radius1 = 30.0f;
    const float radius2 = 20.0f;
    InitWindow(screenWidth, screenHeight, "Physics Engine - Raylib Active");

    SetTargetFPS(60);

    Vector2 ballPos = { 0, 0 };
    Vector2 ball2Pos = { 0, 0 };

    while (!WindowShouldClose()) { 

        if (IsKeyDown(KEY_RIGHT)) ballPos.x += 5.0f;
        if (IsKeyDown(KEY_LEFT))  ballPos.x -= 5.0f;
        if (IsKeyDown(KEY_UP))    ballPos.y -= 5.0f;
        if (IsKeyDown(KEY_DOWN))  ballPos.y += 5.0f;

        if (IsKeyDown(KEY_D))  ball2Pos.x += 5.0f;
        if (IsKeyDown(KEY_A))  ball2Pos.x -= 5.0f;
        if (IsKeyDown(KEY_W))  ball2Pos.y -= 5.0f;
        if (IsKeyDown(KEY_S))  ball2Pos.y += 5.0f;

        BeginDrawing();
            ClearBackground(DARKGRAY);

            DrawText("My physics engine! Use Arrow Keys to Move", 10, 10, 20, RAYWHITE);
            DrawCircleV(ballPos, radius1, RED);
            DrawCircleV(ball2Pos, radius2, BLUE);
            DrawRectangle(0, 500, 800, 100, BLACK);

            DrawFPS(10, 570);
        EndDrawing();

        if(Intercepting(radius1, radius2, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y)) std::cout << "Intercepting" << std::endl;
        else std::cout << "Not intercepting" << std::endl;
    }

    CloseWindow();
    return 0;
}