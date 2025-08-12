#include "raylib.h"
#include <iostream>
#include <stdlib.h>

#define MAP_TILE_SIZE 100
#define MAP_SIZE_X 15
#define MAP_SIZE_Y 9

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

int screenWidth = DEFAULT_WINDOW_WIDTH;
int screenHeight = DEFAULT_WINDOW_HEIGHT;
int displayWidth = DEFAULT_WINDOW_WIDTH;
int displayHeight = DEFAULT_WINDOW_HEIGHT;
int trueMonitorWidth = DEFAULT_WINDOW_WIDTH;
int trueMonitorHeight = DEFAULT_WINDOW_HEIGHT;
Vector2 screenCenter = { DEFAULT_WINDOW_WIDTH / 2.0f, DEFAULT_WINDOW_HEIGHT / 2.0f };

typedef struct Map {
    unsigned int tilesX;
    unsigned int tilesY;
    unsigned char *tileIds;
} Map;

void UpdateScreenDimensions() {
    int currentMonitor = GetCurrentMonitor();
    
    if (IsWindowFullscreen()) {
        screenWidth = trueMonitorWidth;
        screenHeight = trueMonitorHeight;
        displayWidth = trueMonitorWidth;
        displayHeight = trueMonitorHeight;
    } else {
        trueMonitorWidth = GetMonitorWidth(currentMonitor);
        trueMonitorHeight = GetMonitorHeight(currentMonitor);
        displayWidth = trueMonitorWidth;
        displayHeight = trueMonitorHeight;
        
        screenWidth = GetRenderWidth();
        screenHeight = GetRenderHeight();
    }
    
    screenCenter = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
}

void InitDisplaySystem() {
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "FACTOR E");
    
    int primaryMonitor = 0;
    trueMonitorWidth = GetMonitorWidth(primaryMonitor);
    trueMonitorHeight = GetMonitorHeight(primaryMonitor);
    
    displayWidth = trueMonitorWidth;
    displayHeight = trueMonitorHeight;
    
    SetWindowState(FLAG_WINDOW_UNDECORATED);
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    ClearWindowState(FLAG_WINDOW_RESIZABLE);
    
    SetWindowPosition(
        (trueMonitorWidth - DEFAULT_WINDOW_WIDTH) / 2,
        (trueMonitorHeight - DEFAULT_WINDOW_HEIGHT) / 2
    );
    
    BeginDrawing();
    EndDrawing();
    
    SetWindowState(FLAG_FULLSCREEN_MODE);
    
    for (int i = 0; i < 5; i++) {
        BeginDrawing();
        EndDrawing();
    }
    
    UpdateScreenDimensions();
}

int main() {
    InitDisplaySystem();
    
    SetTargetFPS(144);

    Font fontLarge = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 128, 0, 250);
    Font fontMedium = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 64, 0, 250);
    Font fontSmall = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 32, 0, 250);
    SetTextLineSpacing(16);

    Map map = { 0 };
    map.tilesX = MAP_SIZE_X;
    map.tilesY = MAP_SIZE_Y;

    float offsetX = (screenWidth - MAP_TILE_SIZE * MAP_SIZE_X) / 2;
    float offsetY = (screenHeight - MAP_TILE_SIZE * MAP_SIZE_Y) / 2;

    map.tileIds = (unsigned char *)calloc(map.tilesX*map.tilesY, sizeof(unsigned char));
    for (unsigned int i = 0; i < map.tilesY*map.tilesX; i++) map.tileIds[i] = i;

    while (!WindowShouldClose())
    {
        UpdateScreenDimensions();

        offsetX = (screenWidth - MAP_TILE_SIZE * MAP_SIZE_X) / 2;
        offsetY = (screenHeight - MAP_TILE_SIZE * MAP_SIZE_Y) / 2;

        BeginDrawing();

        ClearBackground(BLACK);

        for (unsigned int y = 0; y < map.tilesY; y++)
        {
            for (unsigned int x = 0; x < map.tilesX; x++)
            {
                DrawRectangleLines(offsetX + x*MAP_TILE_SIZE, offsetY + y*MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE, WHITE);
            }
        }

        DrawCircle((int)screenCenter.x, (int)screenCenter.y, 50.0f, WHITE);

        DrawTextEx(fontSmall, TextFormat("Offset: %.2fx%.2f", offsetX, offsetY), (Vector2){ 0, 0 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Monitor Count: %d", GetMonitorCount()), (Vector2){ 0, 25 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Current Monitor: %d", GetCurrentMonitor()), (Vector2){ 0, 50 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Screen: %dx%d", screenWidth, screenHeight), (Vector2){ 0, 75 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Display Size: %dx%d", displayWidth, displayHeight), (Vector2){ 0, 100 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("True Monitor: %dx%d", trueMonitorWidth, trueMonitorHeight), (Vector2){ 0, 125 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Screen Center: %.0fx%.0f", screenCenter.x, screenCenter.y), (Vector2){ 0, 150 }, (float)fontSmall.baseSize, 2, WHITE);
        DrawTextEx(fontSmall, TextFormat("Fullscreen: %s", IsWindowFullscreen() ? "Yes" : "No"), (Vector2){ 0, 175 }, (float)fontSmall.baseSize, 2, WHITE);

        EndDrawing();
    }

    UnloadFont(fontLarge);
    UnloadFont(fontMedium);
    UnloadFont(fontSmall);

    free(map.tileIds);

    CloseWindow();
    return 0;
}