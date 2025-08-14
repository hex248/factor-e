#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720
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

void LoadConfig();
void SaveConfig();
void UpdateConfig();
void UpdateScreenDimensions();
void InitDisplaySystem();

#endif