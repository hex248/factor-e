#include "raylib.h"
#include <iostream>

int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "FACTOR E");

    SetTargetFPS(144);

    SetWindowState(FLAG_WINDOW_UNDECORATED);
    ToggleFullscreen();

    Font fontLarge = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 128, 0, 250);
    Font fontMedium = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 64, 0, 250);
    Font fontSmall = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 32, 0, 250);
    SetTextLineSpacing(16);

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

        // DrawFPS(10, 10);
        DrawTextEx(fontSmall, "FACTOR E", (Vector2){ 0, 0 }, (float)fontSmall.baseSize, 2, WHITE);

        EndDrawing();
    }

    UnloadFont(fontLarge);
    UnloadFont(fontMedium);
    UnloadFont(fontSmall);

    CloseWindow();
    return 0;
}