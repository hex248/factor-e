#include "ui.h"
#include "config.h"
#include "controls.h"
#include "player.h"
#include <stdio.h>
#include "mouse.h"
#include <map>
#include <vector>

static Font fontLargeBold;
static Font fontLarge;
static Font fontLargeExtraLight;
static Font fontMediumBold;
static Font fontMedium;
static Font fontMediumExtraLight;
static Font fontSmallBold;
static Font fontSmall;
static Font fontSmallExtraLight;
static bool fontsLoaded = false;

float toolbeltSpriteScale = 8.0f;
Texture2D toolbeltSprite;
Texture2D toolbeltSlotSelectedSprite;
Vector2 toolbeltPosition = {0, 0};

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

    Image toolbeltSpriteImage = LoadImage("assets/sprites/toolbelt.png");
    ImageResizeNN(&toolbeltSpriteImage,
                  (int)((float)toolbeltSpriteImage.width * toolbeltSpriteScale),
                  (int)((float)toolbeltSpriteImage.height * toolbeltSpriteScale));
    toolbeltSprite = LoadTextureFromImage(toolbeltSpriteImage);
    UnloadImage(toolbeltSpriteImage);

    Image toolbeltSlotSelectedSpriteImage = LoadImage("assets/sprites/toolbelt-slot-selected.png");
    ImageResizeNN(&toolbeltSlotSelectedSpriteImage,
                  (int)((float)toolbeltSlotSelectedSpriteImage.width * toolbeltSpriteScale),
                  (int)((float)toolbeltSlotSelectedSpriteImage.height * toolbeltSpriteScale));
    toolbeltSlotSelectedSprite = LoadTextureFromImage(toolbeltSlotSelectedSpriteImage);
    UnloadImage(toolbeltSlotSelectedSpriteImage);

    toolbeltPosition = {(VIRTUAL_WIDTH - toolbeltSprite.width) / 2.0f, VIRTUAL_HEIGHT * 0.82f};

    fontLargeBold = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Bold.ttf", 128, 0, 250);
    fontLarge = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 128, 0, 250);
    fontLargeExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 128, 0, 250);

    fontMediumBold = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Bold.ttf", 64, 0, 250);
    fontMedium = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 64, 0, 250);
    fontMediumExtraLight = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf", 64, 0, 250);

    fontSmallBold = LoadFontEx("assets/fonts/JetBrainsMono/JetBrainsMono-Bold.ttf", 32, 0, 250);
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

float GetScaledFontSize(float baseFontSize)
{
    return baseFontSize * uiScale;
}

float GetScaledLineHeight(float baseLineHeight)
{
    return baseLineHeight * uiScale;
}

float GetScaledPadding(float basePadding)
{
    return basePadding * uiScale;
}

void DrawToolBelt(const Player &player)
{
    DrawTextureV(toolbeltSprite, toolbeltPosition, WHITE);

    float gap = (2 * toolbeltSpriteScale);
    Vector2 startPos = {toolbeltPosition.x + 3 * toolbeltSpriteScale, toolbeltPosition.y + gap};

    for (int i = 0; i < 7; i++)
    {
        Vector2 itemPos = {startPos.x + (i * (ITEM_STACK_WIDTH + gap)), startPos.y};

        if (i == player.selectedSlot)
        {
            DrawTextureV(toolbeltSlotSelectedSprite, itemPos, WHITE);
        }

        const ItemStack &stack = player.inventory[i];
        if (stack.quantity > 0)
        {
            DrawTextureV(stack.iconSprite, itemPos, WHITE);

            if (stack.quantity > 1)
            {
                const char *quantityText = TextFormat("x%d", stack.quantity);
                unsigned int fontSize = 42;
                unsigned int fontSpacing = 5;
                Vector2 textSize = MeasureTextEx(fontLargeBold, quantityText, fontSize, fontSpacing);
                DrawTextEx(fontLargeBold, quantityText, {itemPos.x + (ITEM_STACK_WIDTH * 1.0f) - (textSize.x), itemPos.y + (ITEM_STACK_HEIGHT * 0.85f) - (textSize.y * 0.5f)}, fontSize, fontSpacing, WHITE);
            }
        }
    }
}

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

    snprintf(buffer, sizeof(buffer), "Screen: %.0fx%.0f (Virtual: %.0fx%.0f)", screenWidth, screenHeight, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    debugValues["screen_size"] = buffer;

    snprintf(buffer, sizeof(buffer), "Display Size: %.0fx%.0f", displayWidth, displayHeight);
    debugValues["display_size"] = buffer;

    snprintf(buffer, sizeof(buffer), "Screen Center: %.0fx%.0f", screenCenter.x, screenCenter.y);
    debugValues["screen_center"] = buffer;

    snprintf(buffer, sizeof(buffer), "Player Position: X:%.1f Y:%.1f", player.position.x, player.position.y);
    debugValues["player_position"] = buffer;

    snprintf(buffer, sizeof(buffer), "Player Reach: %d Tiles", PLAYER_REACH);
    debugValues["player_reach"] = buffer;

    std::vector<std::pair<std::string, Vector2>> debugLines;
    float maxWidth = 0.0f;
    const float lineHeight = GetScaledLineHeight(25.0f);
    const float padding = GetScaledPadding(10.0f);
    const float fontSize = GetScaledFontSize((float)fontSmallExtraLight.baseSize);
    const float spacing = GetScaledPadding(2.0f);

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
            Vector2 textSize = MeasureTextEx(fontSmallExtraLight, text, fontSize, spacing);
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
            DrawTextEx(fontSmallExtraLight, text, {padding, ((float)i * lineHeight) + (padding / 2)}, fontSize, spacing, WHITE);
        }
    }

    DrawControlsInfo();
}

void DrawControlsInfo()
{
    if (!showDebug)
        return;

    const char *controls = "CTRL + R: Reload World | CTRL + SHIFT + R: Regenerate World | F3: Debug | F10: Switch Monitor | F11: Borderless";
    const float fontSize = GetScaledFontSize((float)fontSmallExtraLight.baseSize);
    const float spacing = GetScaledPadding(2.0f);
    const float padding = GetScaledPadding(10.0f);

    Vector2 controlsSize = MeasureTextEx(fontSmallExtraLight, controls, fontSize, spacing);
    DrawTextEx(fontSmallExtraLight, controls,
               {screenWidth - controlsSize.x - padding, screenHeight - controlsSize.y - padding},
               fontSize, spacing, WHITE);
}

void DrawExitConfirmation()
{
    ShowMouse();
    DrawRectangle(0, 0, (int)screenWidth, (int)screenHeight, {0, 0, 0, 200});
    const char *closeText = "close the window? (Y/n)";
    const float fontSize = GetScaledFontSize((float)fontMedium.baseSize);
    const float spacing = GetScaledPadding(2.0f);

    Vector2 textSize = MeasureTextEx(fontMedium, closeText, fontSize, spacing);
    DrawTextEx(fontMedium, closeText,
               {screenWidth / 2 - textSize.x / 2,
                screenHeight / 2 - textSize.y / 2},
               fontSize, spacing, WHITE);
}