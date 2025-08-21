#include "ui.h"
#include "config.h"
#include "controls.h"
#include "player.h"
#include <stdio.h>
#include "mouse.h"
#include <map>
#include <vector>

static Font fontLarge;
static Font fontLargeExtraLight;
static Font fontMedium;
static Font fontMediumExtraLight;
static Font fontSmall;
static Font fontSmallExtraLight;
static bool fontsLoaded = false;

static std::map<std::string, std::string> debugValues;
static std::vector<std::string> debugOrder = {
    "monitor_count",
    "current_monitor",
    "screen_size",
    "display_size",
    "screen_center",
    "SPACER",
    "player_position",
    "SPACER",
    "mouse_position_screen",
    "mouse_position_world",
    "mouse_distance",
    "mouse_distance_tiles",
    "player_reach",
    "hovered_tile"};
static bool debugSystemInitialized = false;

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

void InitDebugSystem()
{
    if (debugSystemInitialized)
        return;

    for (const std::string &key : debugOrder)
    {
        debugValues[key] = "";
    }

    debugSystemInitialized = true;
}

void SetDebugValue(const std::string &key, const std::string &value)
{
    if (!debugSystemInitialized)
        InitDebugSystem();

    debugValues[key] = value;
}

void DrawDebugInfo()
{
    if (!showDebug)
        return;

    if (!debugSystemInitialized)
        InitDebugSystem();

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Monitor Count: %d", GetMonitorCount());
    debugValues["monitor_count"] = buffer;

    snprintf(buffer, sizeof(buffer), "Current Monitor: %d", GetCurrentMonitor());
    debugValues["current_monitor"] = buffer;

    snprintf(buffer, sizeof(buffer), "Screen: %dx%d", screenWidth, screenHeight);
    debugValues["screen_size"] = buffer;

    snprintf(buffer, sizeof(buffer), "Display Size: %dx%d", displayWidth, displayHeight);
    debugValues["display_size"] = buffer;

    snprintf(buffer, sizeof(buffer), "Screen Center: %.0fx%.0f", screenCenter.x, screenCenter.y);
    debugValues["screen_center"] = buffer;

    snprintf(buffer, sizeof(buffer), "Player Position: X:%.1f Y:%.1f", player.position.x, player.position.y);
    debugValues["player_position"] = buffer;

    snprintf(buffer, sizeof(buffer), "Player Reach: %d Tiles", PLAYER_REACH);
    debugValues["player_reach"] = buffer;

    std::vector<std::pair<std::string, Vector2>> debugLines;
    float maxWidth = 0.0f;
    const float lineHeight = 25.0f;
    const float padding = 10.0f;

    for (const std::string &key : debugOrder)
    {
        if (key == "SPACER")
        {
            debugLines.push_back({"", {0, 0}});
            continue;
        }

        auto it = debugValues.find(key);
        if (it != debugValues.end() && !it->second.empty())
        {
            const char *text = it->second.c_str();
            Vector2 textSize = MeasureTextEx(fontSmallExtraLight, text, (float)fontSmallExtraLight.baseSize, 2);
            debugLines.push_back({it->second, textSize});
            if (textSize.x > maxWidth)
                maxWidth = textSize.x;
        }
    }

    // draw debug background
    if (!debugLines.empty())
    {
        float backgroundWidth = maxWidth + (padding * 2);
        float backgroundHeight = ((float)debugLines.size() * lineHeight) + padding;
        DrawRectangle(0, 0, (int)backgroundWidth, (int)backgroundHeight, {0, 0, 0, 120});
    }

    // draw text
    for (size_t i = 0; i < debugLines.size(); i++)
    {
        if (!debugLines[i].first.empty())
        {
            const char *text = debugLines[i].first.c_str();
            DrawTextEx(fontSmallExtraLight, text, {padding, ((float)i * lineHeight) + (padding / 2)}, (float)fontSmallExtraLight.baseSize, 2, WHITE);
        }
    }

    DrawControlsInfo();
}

void DrawControlsInfo()
{
    if (!showDebug)
        return;

    const char *controls = "CTRL + R: Reload Game | F3: Debug | F10: Switch Monitor | F11: Borderless";
    Vector2 controlsSize = MeasureTextEx(fontSmallExtraLight, controls, (float)fontSmallExtraLight.baseSize, 2);
    DrawTextEx(fontSmallExtraLight, controls,
               {(float)screenWidth - controlsSize.x - 10, (float)screenHeight - controlsSize.y - 10},
               (float)fontSmallExtraLight.baseSize, 2, WHITE);
}

void DrawExitConfirmation()
{
    ShowMouse();
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 200});
    const char *closeText = "close the window? (Y/n)";
    int fontSize = fontMedium.baseSize;
    Vector2 textSize = MeasureTextEx(fontMedium, closeText, (float)fontSize, 2.0f);
    DrawTextEx(fontMedium, closeText,
               {(float)screenWidth / 2 - textSize.x / 2,
                (float)screenHeight / 2 - textSize.y / 2},
               (float)fontSize, 2.0f, WHITE);
}