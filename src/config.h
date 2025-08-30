#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

#define DEFAULT_WINDOW_WIDTH 1280.0f
#define DEFAULT_WINDOW_HEIGHT 720.0f
#define VIRTUAL_WIDTH 2560.0f
#define VIRTUAL_HEIGHT 1440.0f
#define CONFIG_FILE_NAME "factor-e.config"

typedef struct Config
{
    bool borderlessFullscreen;
    int preferredMonitor;
    float windowWidth;
    float windowHeight;
    int windowPosX;
    int windowPosY;
} Config;

extern Config config;
extern bool configChanged;

extern float screenWidth;
extern float screenHeight;
extern float displayWidth;
extern float displayHeight;
extern float trueMonitorWidth;
extern float trueMonitorHeight;
extern Vector2 screenCenter;

extern RenderTexture2D virtualScreen;
extern Rectangle virtualRect;
extern Rectangle targetRect;
extern Vector2 virtualScale;
extern float uiScale;

void LoadConfig();
void SaveConfig();
void UpdateConfig();
void UpdateScreenDimensions();
void InitDisplaySystem();
void InitVirtualScreen();
void UpdateVirtualScreen();
void CleanupVirtualScreen();

#endif
