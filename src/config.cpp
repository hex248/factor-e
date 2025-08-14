#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config config = {0};
bool configChanged = false;

int screenWidth = DEFAULT_WINDOW_WIDTH;
int screenHeight = DEFAULT_WINDOW_HEIGHT;
int displayWidth = DEFAULT_WINDOW_WIDTH;
int displayHeight = DEFAULT_WINDOW_HEIGHT;
int trueMonitorWidth = DEFAULT_WINDOW_WIDTH;
int trueMonitorHeight = DEFAULT_WINDOW_HEIGHT;
Vector2 screenCenter = {DEFAULT_WINDOW_WIDTH / 2.0f, DEFAULT_WINDOW_HEIGHT / 2.0f};

void SaveConfig()
{
    FILE *file = fopen(CONFIG_FILE_NAME, "w");
    if (file != NULL)
    {
        fprintf(file, "borderlessFullscreen=%d\n", config.borderlessFullscreen ? 1 : 0);
        fprintf(file, "preferredMonitor=%d\n", config.preferredMonitor);
        fprintf(file, "windowWidth=%d\n", config.windowWidth);
        fprintf(file, "windowHeight=%d\n", config.windowHeight);
        fprintf(file, "windowPosX=%d\n", config.windowPosX);
        fprintf(file, "windowPosY=%d\n", config.windowPosY);
        fclose(file);
        configChanged = false;
    }
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

    FILE *file = fopen(CONFIG_FILE_NAME, "r");
    if (file != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), file))
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
        }
        fclose(file);
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
    {
        configChanged = true;
    }
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

    screenCenter = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
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
        int monitorX = GetMonitorPosition(monitor).x;
        int monitorY = GetMonitorPosition(monitor).y;
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
}
