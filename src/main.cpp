#include "raylib.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#define DEBUG_MODE 1

#define MAP_TILE_SIZE 100
#define MAP_SIZE_X 5
#define MAP_SIZE_Y 5

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

int screenWidth = DEFAULT_WINDOW_WIDTH;
int screenHeight = DEFAULT_WINDOW_HEIGHT;
int displayWidth = DEFAULT_WINDOW_WIDTH;
int displayHeight = DEFAULT_WINDOW_HEIGHT;
int trueMonitorWidth = DEFAULT_WINDOW_WIDTH;
int trueMonitorHeight = DEFAULT_WINDOW_HEIGHT;
Vector2 screenCenter = {DEFAULT_WINDOW_WIDTH / 2.0f, DEFAULT_WINDOW_HEIGHT / 2.0f};

typedef struct Tile
{
    unsigned char id;
    char name[16];
    Color color;
} Tile;

typedef struct Map
{
    unsigned int tilesX;
    unsigned int tilesY;
    unsigned char *tileIds;
    Tile *tiles;
} Map;

void UpdateScreenDimensions()
{
    int monitor = GetCurrentMonitor();

    if (IsWindowFullscreen())
    {
        screenWidth = trueMonitorWidth;
        screenHeight = trueMonitorHeight;
        displayWidth = trueMonitorWidth;
        displayHeight = trueMonitorHeight;
    }
    else
    {
        trueMonitorWidth = GetMonitorWidth(monitor);
        trueMonitorHeight = GetMonitorHeight(monitor);
        displayWidth = trueMonitorWidth;
        displayHeight = trueMonitorHeight;

        screenWidth = GetRenderWidth();
        screenHeight = GetRenderHeight();
    }

    screenCenter = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
}

void InitDisplaySystem()
{
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "FACTOR E");

    int monitor = GetCurrentMonitor();

    trueMonitorWidth = GetMonitorWidth(monitor);
    trueMonitorHeight = GetMonitorHeight(monitor);

    displayWidth = trueMonitorWidth;
    displayHeight = trueMonitorHeight;

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    // ClearWindowState(FLAG_WINDOW_RESIZABLE);

    SetWindowPosition(
        (trueMonitorWidth - DEFAULT_WINDOW_WIDTH) / 2,
        (trueMonitorHeight - DEFAULT_WINDOW_HEIGHT) / 2);

    UpdateScreenDimensions();
}

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
    map.tilesX = MAP_SIZE_X;
    map.tilesY = MAP_SIZE_Y;

    float offsetX = (screenWidth - MAP_TILE_SIZE * MAP_SIZE_X) / 2;
    float offsetY = (screenHeight - MAP_TILE_SIZE * MAP_SIZE_Y) / 2;

    map.tileIds = (unsigned char *)calloc(map.tilesX * map.tilesY, sizeof(unsigned char));
    for (unsigned int i = 0; i < map.tilesY * map.tilesX; i++)
        map.tileIds[i] = i;

    map.tiles = (Tile *)calloc(map.tilesX * map.tilesY, sizeof(Tile));
    for (unsigned int i = 0; i < map.tilesY * map.tilesX; i++)
    {
        map.tiles[i].id = i;
        snprintf(map.tiles[i].name, sizeof(map.tiles[i].name), "%d", i + 1);
        int lightness = GetRandomValue(0, 120);
        map.tiles[i].color = (Color){lightness, lightness + 80, lightness, 255};
    }

    while (!WindowShouldClose())
    {
        UpdateScreenDimensions();

        if (IsKeyPressed(KEY_F11))
            ToggleBorderlessWindowed();

        offsetX = (screenWidth - MAP_TILE_SIZE * MAP_SIZE_X) / 2;
        offsetY = (screenHeight - MAP_TILE_SIZE * MAP_SIZE_Y) / 2;

        BeginDrawing();

        ClearBackground((Color){10, 10, 10, 255});

        for (unsigned int y = 0; y < map.tilesY; y++)
        {
            for (unsigned int x = 0; x < map.tilesX; x++)
            {
                int tileX = (int)(offsetX + x * MAP_TILE_SIZE);
                int tileY = (int)(offsetY + y * MAP_TILE_SIZE);

                DrawRectangle(tileX, tileY, MAP_TILE_SIZE, MAP_TILE_SIZE, map.tiles[y * map.tilesX + x].color);

                Vector2 textSize = MeasureTextEx(fontSmall, map.tiles[y * map.tilesX + x].name, (float)fontSmall.baseSize, 2);
                Vector2 textPos = {
                    tileX + (MAP_TILE_SIZE - textSize.x) / 2,
                    tileY + (MAP_TILE_SIZE - textSize.y) / 2};
                DrawTextEx(fontSmall, map.tiles[y * map.tilesX + x].name, textPos, (float)fontSmall.baseSize, 2, WHITE);
            }
        }

        int gridStartX = (int)offsetX;
        int gridStartY = (int)offsetY;
        int gridEndX = gridStartX + MAP_SIZE_X * MAP_TILE_SIZE;
        int gridEndY = gridStartY + MAP_SIZE_Y * MAP_TILE_SIZE;

        for (unsigned int x = 0; x <= MAP_SIZE_X; x++)
        {
            int lineX = gridStartX + x * MAP_TILE_SIZE;
            DrawLine(lineX, gridStartY, lineX, gridEndY, WHITE);
        }

        for (unsigned int y = 0; y <= MAP_SIZE_Y; y++)
        {
            int lineY = gridStartY + y * MAP_TILE_SIZE;
            DrawLine(gridStartX, lineY, gridEndX, lineY, WHITE);
        }

        DrawCircle((int)screenCenter.x, (int)screenCenter.y, 30.0f, WHITE);

        if (DEBUG_MODE)
        {
            static char debugText[6][64];
            snprintf(debugText[0], sizeof(debugText[0]), "Offset: %dx%d", (int)offsetX, (int)offsetY);
            snprintf(debugText[1], sizeof(debugText[1]), "Monitor Count: %d", GetMonitorCount());
            snprintf(debugText[2], sizeof(debugText[2]), "Current Monitor: %d", GetCurrentMonitor());
            snprintf(debugText[3], sizeof(debugText[3]), "Screen: %dx%d", screenWidth, screenHeight);
            snprintf(debugText[4], sizeof(debugText[4]), "Display Size: %dx%d", displayWidth, displayHeight);
            snprintf(debugText[5], sizeof(debugText[5]), "Screen Center: %.0fx%.0f", screenCenter.x, screenCenter.y);

            for (int i = 0; i < 6; i++)
            {
                DrawTextEx(fontSmallExtraLight, debugText[i], (Vector2){0, i * 25}, (float)fontSmallExtraLight.baseSize, 2, WHITE);
            }
        }

        EndDrawing();
    }

    UnloadFont(fontLarge);
    UnloadFont(fontLargeExtraLight);
    UnloadFont(fontMedium);
    UnloadFont(fontMediumExtraLight);
    UnloadFont(fontSmall);
    UnloadFont(fontSmallExtraLight);

    free(map.tileIds);

    CloseWindow();
    return 0;
}