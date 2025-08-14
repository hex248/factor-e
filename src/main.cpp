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

    Player player = Player(Vector2{screenWidth / 2.0f, screenHeight / 2.0f}, 25.0f, PLAYER_SPEED, BLUE);

    Camera2D camera = {0};
    camera.target = (Vector2){player.position.x, player.position.y};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        UpdateScreenDimensions();
        UpdateConfig();

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