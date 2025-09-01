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
#include "tex.h"

void ReloadGame()
{
    player.position = {0.0f, 0.0f};
    player.dir = {0, 0};
}

void RegenerateWorld()
{
    GenerateWorld();

    ReloadGame();
}

int main()
{
    SetTraceLogLevel(LOG_ERROR);

    InitDisplaySystem();
    InitialiseMouse();

    SetTargetFPS(144);

    InitFonts();
    InitDebugSystem();

    InitWorld();

    if (!DEV)
        SetExitKey(KEY_NULL);
    bool exitWindowRequested = false;
    bool exitWindow = false;
    const int CLOSE_KEY = KEY_ESCAPE;

    player.Initialize({0.0f, 0.0f}, 25.0f, PLAYER_SPEED);

    Camera2D camera;
    camera.target = {player.position.x, player.position.y};
    camera.offset = {(float)VIRTUAL_WIDTH / 2.0f, (float)VIRTUAL_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!exitWindow)
    {
        UpdateScreenDimensions();
        UpdateConfig();

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

        // RENDER TO VIRTUAL SCREEN
        BeginTextureMode(virtualScreen);
        ClearBackground({10, 10, 10, 255});

        BeginMode2D(camera);
        DrawWorld();
        CheckHover();
        player.Draw();

        EndMode2D();

        DrawToolBelt();

        DrawMouse();

        EndTextureMode();

        // RENDER VIRTUAL SCREEN TO ACTUAL SCREEN
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(virtualScreen.texture, virtualRect, targetRect, {0, 0}, 0.0f, WHITE);

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
    CleanupVirtualScreen();
    CleanupFonts();

    CleanupWorld();

    UnregisterTextures();

    if (configChanged)
        SaveConfig();

    CloseWindow();
    return 0;
}