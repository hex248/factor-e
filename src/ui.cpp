#include "ui.h"
#include "config.h"
#include "controls.h"
#include <stdio.h>
#include "mouse.h"

static Font fontLarge;
static Font fontLargeExtraLight;
static Font fontMedium;
static Font fontMediumExtraLight;
static Font fontSmall;
static Font fontSmallExtraLight;
static bool fontsLoaded = false;

void InitFonts()
{
    if (fontsLoaded)
        return;

    fontLarge = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 128, 0, 250);
    fontLargeExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 128, 0, 250);
    fontMedium = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 64, 0, 250);
    fontMediumExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 64, 0, 250);
    fontSmall = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 32, 0, 250);
    fontSmallExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 32, 0, 250);

    SetTextLineSpacing(16);
    fontsLoaded = true;
}

void CleanupFonts()
{
    if (!fontsLoaded)
        return;

    UnloadFont(fontLarge);
    UnloadFont(fontLargeExtraLight);
    UnloadFont(fontMedium);
    UnloadFont(fontMediumExtraLight);
    UnloadFont(fontSmall);
    UnloadFont(fontSmallExtraLight);
    fontsLoaded = false;
}

Font GetFontLarge() { return fontLarge; }
Font GetFontLargeExtraLight() { return fontLargeExtraLight; }
Font GetFontMedium() { return fontMedium; }
Font GetFontMediumExtraLight() { return fontMediumExtraLight; }
Font GetFontSmall() { return fontSmall; }
Font GetFontSmallExtraLight() { return fontSmallExtraLight; }

void DrawDebugInfo()
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
        DrawTextEx(fontSmallExtraLight, debugText[i], (Vector2){0, i * 25.0f}, (float)fontSmallExtraLight.baseSize, 2, WHITE);
    }
}

void DrawControlsInfo()
{
    if (!showControls)
        return;

    const char *controls = "F1: Hide Controls | F2: Debug | F10: Switch Monitor | F11: Borderless";
    Vector2 controlsSize = MeasureTextEx(fontSmallExtraLight, controls, (float)fontSmallExtraLight.baseSize, 2);
    DrawTextEx(fontSmallExtraLight, controls,
               (Vector2){screenWidth - controlsSize.x - 10, screenHeight - controlsSize.y - 10},
               (float)fontSmallExtraLight.baseSize, 2, WHITE);
}

void DrawExitConfirmation()
{
    ShowMouse();
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