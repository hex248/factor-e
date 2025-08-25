#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900
#define VIRTUAL_WIDTH 1920
#define VIRTUAL_HEIGHT 1080
#define CONFIG_FILE_NAME "factor-e.config"

typedef struct Config
{
    bool borderlessFullscreen;
    int preferredMonitor;
    int windowWidth;
    int windowHeight;
    int windowPosX;
    int windowPosY;
} Config;

extern Config config;
extern bool configChanged;

extern int screenWidth;
extern int screenHeight;
extern int displayWidth;
extern int displayHeight;
extern int trueMonitorWidth;
extern int trueMonitorHeight;
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