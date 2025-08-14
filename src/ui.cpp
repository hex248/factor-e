#include "ui.h"
#include "config.h"
#include "controls.h"
#include <stdio.h>

void DrawDebugInfo(Font font)
{
    if (!showDebug)
        return;

    static char debugText[5][64];

    snprintf(debugText[0], sizeof(debugText[1]), "Monitor Count: %d", GetMonitorCount());
    snprintf(debugText[1], sizeof(debugText[2]), "Current Monitor: %d", GetCurrentMonitor());
    snprintf(debugText[2], sizeof(debugText[3]), "Screen: %dx%d", screenWidth, screenHeight);
    snprintf(debugText[3], sizeof(debugText[4]), "Display Size: %dx%d", displayWidth, displayHeight);
    snprintf(debugText[4], sizeof(debugText[4]), "Screen Center: %.0fx%.0f", screenCenter.x, screenCenter.y);

    for (int i = 0; i < 5; i++)
    {
        DrawTextEx(font, debugText[i], (Vector2){0, i * 25.0f}, (float)font.baseSize, 2, WHITE);
    }
}

void DrawControlsInfo(Font font)
{
    if (!showControls)
        return;

    const char *controls = "F1: Hide Controls | F2: Debug | F10: Switch Monitor | F11: Borderless";
    Vector2 controlsSize = MeasureTextEx(font, controls, (float)font.baseSize, 2);
    DrawTextEx(font, controls,
               (Vector2){screenWidth - controlsSize.x - 10, screenHeight - controlsSize.y - 10},
               (float)font.baseSize, 2, WHITE);
}
