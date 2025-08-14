#ifndef UI_H
#define UI_H

#include "raylib.h"

void InitFonts();
void CleanupFonts();

Font GetFontLarge();
Font GetFontLargeExtraLight();
Font GetFontMedium();
Font GetFontMediumExtraLight();
Font GetFontSmall();
Font GetFontSmallExtraLight();

void DrawDebugInfo();
void DrawControlsInfo();
void DrawExitConfirmation();

#endif