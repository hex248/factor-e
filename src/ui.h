#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <string>

class Player;

void InitFonts();
void CleanupFonts();

Font GetFontLarge();
Font GetFontLargeExtraLight();
Font GetFontMedium();
Font GetFontMediumExtraLight();
Font GetFontSmall();
Font GetFontSmallExtraLight();

float GetScaledFontSize(float baseFontSize);
float GetScaledLineHeight(float baseLineHeight);
float GetScaledPadding(float basePadding);

void DrawToolBelt();

void InitDebugSystem();
void SetDebugValue(const std::string &key, const std::string &value);
void DrawDebugInfo();
void DrawControlsInfo();
void DrawExitConfirmation();

#endif