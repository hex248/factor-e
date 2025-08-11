#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "FACTOR E");

    SetTargetFPS(144);

    SetWindowState(FLAG_WINDOW_UNDECORATED);
    ToggleFullscreen();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F1))
        {
            int display = GetCurrentMonitor();

            if (IsWindowFullscreen())
            {
                SetWindowSize(screenWidth, screenHeight);
            }
            else
            {
                SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            }

            ToggleFullscreen();
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}