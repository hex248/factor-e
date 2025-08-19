#include "mouse.h"
#include <stdio.h>

Texture2D pointerSprite;
Texture2D handSprite;

static bool showMouse = false;
static bool showCustomCursor = false;
static bool cursorHasBeenDisabled = false;

void InitialiseMouse()
{
    if (showMouse)
        ShowMouse();
    else
        HideMouse();

    showCustomCursor = true;

    Image pointerImage = LoadImage(POINTER_SPRITE_PATH);
    Image handImage = LoadImage(HAND_SPRITE_PATH);

    ImageResizeNN(&pointerImage,
                  (int)(pointerImage.width * CURSOR_SPRITE_SCALE),
                  (int)(pointerImage.height * CURSOR_SPRITE_SCALE));
    ImageResizeNN(&handImage,
                  (int)(handImage.width * CURSOR_SPRITE_SCALE),
                  (int)(handImage.height * CURSOR_SPRITE_SCALE));

    pointerSprite = LoadTextureFromImage(pointerImage);
    UnloadImage(pointerImage);
}

void HandleMouse()
{
    Vector2 mousePos = GetMousePosition();
    // handle hover stuff
}

void DrawMouse()
{
    if (!showCustomCursor)
        return;
    Vector2 mousePos = GetMousePosition();

    mousePos = (Vector2){
        mousePos.x - CURSOR_SPRITE_SCALE,
        mousePos.y - CURSOR_SPRITE_SCALE}; // the cursors have an empty pixel in the top left

    DrawTextureV(pointerSprite, mousePos, WHITE);
}

void HideMouse()
{
    DisableCursor();
    HideCursor();
    showMouse = false;
    showCustomCursor = true;
}

void ShowMouse()
{
    EnableCursor();
    ShowCursor();
    showMouse = true;
    showCustomCursor = false;
}