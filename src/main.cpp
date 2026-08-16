#include <iostream>
#include "raylib.h"
#include "vector2.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float radius1 = 10.0f;
    const float radius2 = 10.0f;
    const float speed = 10.0f;
    const float gravity = 6.0f;
    InitWindow(screenWidth, screenHeight, "Physics Engine - Raylib Active");

    SetTargetFPS(60);

    Vector2 ballPos = { 40, 40 };
    Vector2 ball2Pos = { 100, 100 };


    while (!WindowShouldClose()) { 
        // Ball 1 Movement (Arrow Keys)
        if (IsKeyDown(KEY_RIGHT) && !Collides(radius1, radius2, speed, ballPos.x + speed, ball2Pos.x, ballPos.y, ball2Pos.y)) ballPos.x += speed;
        if (IsKeyDown(KEY_LEFT)  && !Collides(radius1, radius2, speed, ballPos.x - speed, ball2Pos.x, ballPos.y, ball2Pos.y)) ballPos.x -= speed;
        if (IsKeyDown(KEY_UP)    && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y - speed, ball2Pos.y)) ballPos.y -= speed;
        if (IsKeyDown(KEY_DOWN)  && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y + speed, ball2Pos.y)) ballPos.y += speed;
    
        // Ball 2 Movement (WASD Keys)
        if (IsKeyDown(KEY_D) && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x + speed, ballPos.y, ball2Pos.y)) ball2Pos.x += speed;
        if (IsKeyDown(KEY_A) && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x - speed, ballPos.y, ball2Pos.y)) ball2Pos.x -= speed;
        if (IsKeyDown(KEY_W) && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y - speed)) ball2Pos.y -= speed;
        if (IsKeyDown(KEY_S) && !Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y + speed)) ball2Pos.y += speed;
    
        BeginDrawing();
            ClearBackground(DARKGRAY);
    
            DrawText("My physics engine! Use Arrow Keys to Move", 10, 10, 20, RAYWHITE);
            DrawCircleV(ballPos, radius1, RED);
            DrawCircleV(ball2Pos, radius2, BLUE);
            
            DrawRectangle(0, -30, screenWidth, 30, BLACK); // Top border
            DrawRectangle(0, screenHeight, screenWidth, 30, BLACK); // Bottom border
            DrawRectangle(-30, 0, 30, screenHeight, BLACK);        // Left border
            DrawRectangle(screenWidth, 0, 30, screenHeight, BLACK); // Right border
    
            DrawFPS(10, 570);
        EndDrawing();
    
        if (Collides(radius1, radius2, speed, ballPos.x, ball2Pos.x, ballPos.y, ball2Pos.y)) 
            std::cout << "Colliding" << std::endl;
        else 
            std::cout << "Not colliding" << std::endl;
    }

    CloseWindow();
    return 0;
}