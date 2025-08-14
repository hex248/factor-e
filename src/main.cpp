#include "raylib.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

// Include our custom modules
#include "config.h"
#include "player.h"
#include "world.h"
#include "controls.h"
#include "ui.h"

int main()
{
    InitDisplaySystem();

    SetTargetFPS(144);

    Font fontLarge = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 128, 0, 250);
    Font fontLargeExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 128, 0, 250);
    Font fontMedium = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 64, 0, 250);
    Font fontMediumExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 64, 0, 250);
    Font fontSmall = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 32, 0, 250);
    Font fontSmallExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 32, 0, 250);

    SetTextLineSpacing(16);

    Map map = {0};
    InitWorld(&map);

    SetExitKey(KEY_NULL);
    bool exitWindowRequested = false;
    bool exitWindow = false;
    const int CLOSE_KEY = KEY_ESCAPE;

    Player player = Player(Vector2{0.0f, 0.0f}, 25.0f, PLAYER_SPEED, (Color){244, 112, 46, 255});

    Camera2D camera = {0};
    camera.target = (Vector2){player.position.x, player.position.y};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!exitWindow)
    {
        UpdateScreenDimensions();
        UpdateConfig();

        camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};

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

        camera.target = (Vector2){player.position.x, player.position.y};

        // RENDER
        BeginDrawing();
        ClearBackground((Color){10, 10, 10, 255});

        BeginMode2D(camera);
        DrawWorld(&map, fontSmall);
        player.Draw();
        EndMode2D();

        DrawDebugInfo(fontSmallExtraLight);
        DrawControlsInfo(fontSmallExtraLight);

        // exit confirmation prompt
        if (WindowShouldClose() || IsKeyPressed(CLOSE_KEY))
        {
            exitWindowRequested = !exitWindowRequested;
        }
        if (exitWindowRequested)
        {
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER))
                exitWindow = true;
            else if (IsKeyPressed(KEY_N))
                exitWindowRequested = false;
        }

        if (exitWindowRequested)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});
            const char *closeText = "close the window? (Y/n)";
            int fontSize = fontMedium.baseSize;
            Vector2 textSize = MeasureTextEx(fontMedium, closeText, fontSize, 2.0f);
            DrawTextEx(fontMedium, closeText,
                       (Vector2){
                           screenWidth / 2 - textSize.x / 2,
                           screenHeight / 2 - textSize.y / 2},
                       fontSize, 2.0f, WHITE);
        }

        EndDrawing();
    }

    // CLEAN UP
    UnloadFont(fontLarge);
    UnloadFont(fontLargeExtraLight);
    UnloadFont(fontMedium);
    UnloadFont(fontMediumExtraLight);
    UnloadFont(fontSmall);
    UnloadFont(fontSmallExtraLight);

    CleanupWorld(&map);

    if (configChanged)
        SaveConfig();

    CloseWindow();
    return 0;
}