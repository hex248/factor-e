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

void InitDebugSystem();
void SetDebugValue(const std::string &key, const std::string &value);
void DrawDebugInfo(Player *player);
void DrawControlsInfo();
void DrawExitConfirmation();

#endif