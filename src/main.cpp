#include <iostream>
#include "raylib.h"
#include "vector2.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float radius1 = 30.0f;
    const float radius2 = 20.0f;
    const float speed = 2.0f;
    const float gravity = 6.0f;
    InitWindow(screenWidth, screenHeight, "Physics Engine - Raylib Active");

    SetTargetFPS(60);

    Vector2 ballPos = { 0, 0 };
    Vector2 ball2Pos = { 100, 100 };


    while (!WindowShouldClose()) { 
        bool collides = Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y);

        if (IsKeyDown(KEY_RIGHT) && !collides) ballPos.x += speed;
        if (IsKeyDown(KEY_LEFT) && !collides)  ballPos.x -= speed;
        if (IsKeyDown(KEY_UP) && !collides)    ballPos.y -= speed;
        if (IsKeyDown(KEY_DOWN) && !collides)  ballPos.y += speed;

        if (IsKeyDown(KEY_D) && !collides)  ball2Pos.x += speed;
        if (IsKeyDown(KEY_A) && !collides)  ball2Pos.x -= speed;
        if (IsKeyDown(KEY_W) && !collides)  ball2Pos.y -= speed;
        if (IsKeyDown(KEY_S) && !collides)  ball2Pos.y += speed;

        // ballPos.y += gravity;
        // ball2Pos.y += gravity;

        BeginDrawing();
            ClearBackground(DARKGRAY);

            DrawText("My physics engine! Use Arrow Keys to Move", 10, 10, 20, RAYWHITE);
            DrawCircleV(ballPos, radius1, RED);
            DrawCircleV(ball2Pos, radius2, BLUE);
            // DrawRectangle(0, 500, 800, 100, BLACK);

            DrawFPS(10, 570);
        EndDrawing();

        if(Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y)) std::cout << "Colliding" << std::endl;
        else std::cout << "Not colliding" << std::endl;
        // collides = false;
    }

    CloseWindow();
    return 0;
}