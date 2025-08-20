#include "main.h"
#include "raylib.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#include "config.h"
#include "player.h"
#include "world.h"
#include "controls.h"
#include "ui.h"
#include "mouse.h"

Map map = {0};
void ReloadGame()
{
    player.position = {0.0f, 0.0f};
    player.dir = {0, 0};

    InitWorld(&map);
}

int main()
{
    InitDisplaySystem();
    InitialiseMouse();

    SetTargetFPS(144);

    InitFonts();
    InitDebugSystem();

    InitWorld(&map);

    if (!DEV)
        SetExitKey(KEY_NULL);
    bool exitWindowRequested = false;
    bool exitWindow = false;
    const int CLOSE_KEY = KEY_ESCAPE;

    player.Initialize({0.0f, 0.0f}, 25.0f, PLAYER_SPEED, WHITE);

    Camera2D camera = {0};
    camera.target = {player.position.x, player.position.y};
    camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!exitWindow)
    {
        UpdateScreenDimensions();
        UpdateConfig();

        camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};

        HandleMouse(camera);
        HandleControls();
        player.HandleMovement();

        // save config if it changed
        static float configSaveTimer = 0.0f;
        if (configChanged)
        {
            configSaveTimer += GetFrameTime();
            // 1 second buffer time for saving to avoid spam
            if (configSaveTimer >= 1.0f)
            {
                SaveConfig();
                configSaveTimer = 0.0f;
            }
        }
        else
        {
            configSaveTimer = 0.0f;
        }

        camera.target = {player.position.x, player.position.y};

        // RENDER
        BeginDrawing();
        ClearBackground({10, 10, 10, 255});

        BeginMode2D(camera);
        DrawWorld(&map);
        CheckHover(&map);
        player.Draw();
        EndMode2D();

        DrawMouse();
        DrawDebugInfo();

        // exit confirmation prompt
        if (WindowShouldClose() || IsKeyPressed(CLOSE_KEY))
        {
            if (!IsCursorHidden())
                HideMouse();
            exitWindowRequested = !exitWindowRequested;
        }
        if (exitWindowRequested)
        {
            if (DEV)
                exitWindow = true;
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER))
                exitWindow = true;
            else if (IsKeyPressed(KEY_N))
            {
                HideMouse();
                exitWindowRequested = false;
            }
        }

        if (exitWindowRequested && !DEV)
        {
            DrawExitConfirmation();
        }

        EndDrawing();
    }

    // CLEAN UP
    CleanupFonts();

    CleanupWorld(&map);

    CleanupCursors();

    if (configChanged)
        SaveConfig();

    CloseWindow();
    return 0;
}