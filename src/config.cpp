#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Config config;
bool configChanged = false;

int screenWidth = DEFAULT_WINDOW_WIDTH;
int screenHeight = DEFAULT_WINDOW_HEIGHT;
int displayWidth = DEFAULT_WINDOW_WIDTH;
int displayHeight = DEFAULT_WINDOW_HEIGHT;
int trueMonitorWidth = DEFAULT_WINDOW_WIDTH;
int trueMonitorHeight = DEFAULT_WINDOW_HEIGHT;
Vector2 screenCenter = {DEFAULT_WINDOW_WIDTH / 2.0f, DEFAULT_WINDOW_HEIGHT / 2.0f};

RenderTexture2D virtualScreen;
Rectangle virtualRect = {0.0f, 0.0f, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT}; // Negative height to flip source
Rectangle targetRect;
Vector2 virtualScale;

void SaveConfig()
{
    char configText[512];
    snprintf(configText, sizeof(configText),
             "borderlessFullscreen=%d\n"
             "preferredMonitor=%d\n"
             "windowWidth=%d\n"
             "windowHeight=%d\n"
             "windowPosX=%d\n"
             "windowPosY=%d\n",
             config.borderlessFullscreen ? 1 : 0,
             config.preferredMonitor,
             config.windowWidth,
             config.windowHeight,
             config.windowPosX,
             config.windowPosY);

    if (SaveFileText(CONFIG_FILE_NAME, configText))
        configChanged = false;
}

void LoadConfig()
{
    // set default values
    config.borderlessFullscreen = false;
    config.preferredMonitor = 0;
    config.windowWidth = DEFAULT_WINDOW_WIDTH;
    config.windowHeight = DEFAULT_WINDOW_HEIGHT;
    config.windowPosX = -1; // -1 is center
    config.windowPosY = -1;

    char *fileText = LoadFileText(CONFIG_FILE_NAME);
    if (fileText != NULL)
    {
        char *line = strtok(fileText, "\n");
        while (line != NULL)
        {
            char key[64], value[64];
            if (sscanf(line, "%63[^=]=%63s", key, value) == 2)
            {
                if (strcmp(key, "borderlessFullscreen") == 0)
                    config.borderlessFullscreen = (atoi(value) != 0);
                else if (strcmp(key, "preferredMonitor") == 0)
                    config.preferredMonitor = atoi(value);
                else if (strcmp(key, "windowWidth") == 0)
                    config.windowWidth = atoi(value);
                else if (strcmp(key, "windowHeight") == 0)
                    config.windowHeight = atoi(value);
                else if (strcmp(key, "windowPosX") == 0)
                    config.windowPosX = atoi(value);
                else if (strcmp(key, "windowPosY") == 0)
                    config.windowPosY = atoi(value);
            }
            line = strtok(NULL, "\n");
        }

        UnloadFileText(fileText);
    }
}

void UpdateConfig()
{
    bool wasChanged = false;

    if (!IsWindowFullscreen())
    {
        Vector2 windowPos = GetWindowPosition();
        if (config.windowPosX != (int)windowPos.x || config.windowPosY != (int)windowPos.y)
        {
            config.windowPosX = (int)windowPos.x;
            config.windowPosY = (int)windowPos.y;
            wasChanged = true;
        }

        int currentWidth = GetRenderWidth();
        int currentHeight = GetRenderHeight();
        if (config.windowWidth != currentWidth || config.windowHeight != currentHeight)
        {
            config.windowWidth = currentWidth;
            config.windowHeight = currentHeight;
            wasChanged = true;
        }
    }

    int currentMonitor = GetCurrentMonitor();
    if (config.preferredMonitor != currentMonitor)
    {
        config.preferredMonitor = currentMonitor;
        wasChanged = true;
    }

    if (wasChanged)
        configChanged = true;
}

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

    screenCenter = {(float)screenWidth / 2.0f, (float)screenHeight / 2.0f};
    
    UpdateVirtualScreen();
}

void InitDisplaySystem()
{
    LoadConfig();

    InitWindow(config.windowWidth, config.windowHeight, "FACTOR E");

    // set preferred monitor
    int monitorCount = GetMonitorCount();
    if (config.preferredMonitor >= monitorCount)
        config.preferredMonitor = 0;

    int monitor = config.preferredMonitor;
    trueMonitorWidth = GetMonitorWidth(monitor);
    trueMonitorHeight = GetMonitorHeight(monitor);

    displayWidth = trueMonitorWidth;
    displayHeight = trueMonitorHeight;

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    if (config.windowPosX == -1 || config.windowPosY == -1)
    {
        // center on preferred monitor
        int monitorX = (int)GetMonitorPosition(monitor).x;
        int monitorY = (int)GetMonitorPosition(monitor).y;
        SetWindowPosition(
            monitorX + (trueMonitorWidth - config.windowWidth) / 2,
            monitorY + (trueMonitorHeight - config.windowHeight) / 2);
    }
    else
    {
        SetWindowPosition(config.windowPosX, config.windowPosY);
    }

    if (config.borderlessFullscreen)
        ToggleBorderlessWindowed();

    UpdateScreenDimensions();
    InitVirtualScreen();
}

void InitVirtualScreen()
{
    virtualScreen = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    
    SetTextureFilter(virtualScreen.texture, TEXTURE_FILTER_POINT);
    
    UpdateVirtualScreen();
}

void UpdateVirtualScreen()
{
    float scaleX = (float)screenWidth / VIRTUAL_WIDTH;
    float scaleY = (float)screenHeight / VIRTUAL_HEIGHT;
    float scale = fminf(scaleX, scaleY);
    
    virtualScale = {scale, scale};
    
    float targetWidth = VIRTUAL_WIDTH * scale;
    float targetHeight = VIRTUAL_HEIGHT * scale;
    
    targetRect = {
        (screenWidth - targetWidth) / 2.0f,
        (screenHeight - targetHeight) / 2.0f,
        targetWidth,
        targetHeight
    };
}

void CleanupVirtualScreen()
{
    UnloadRenderTexture(virtualScreen);
}
