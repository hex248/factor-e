#include "mouse.h"
#include <stdio.h>

Texture2D pointerSprite;
Texture2D handSprite;
Texture2D currentCursorSprite;

static bool showMouse = false;
static bool showCustomCursor = false;
static bool cursorHasBeenDisabled = false;

static Vector2 mouseScreenPos = {0};
static Vector2 mouseWorldPos = {0};

void InitialiseMouse()
{
    SetCurrentCursorSprite("POINTER");
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
    handSprite = LoadTextureFromImage(handImage);
    UnloadImage(handImage);
}

void HandleMouse(Camera2D camera)
{
    mouseScreenPos = GetMousePosition();
    mouseWorldPos = GetScreenToWorld2D(mouseScreenPos, camera);
}

void DrawMouse()
{
    if (!showCustomCursor)
        return;
    Vector2 mousePos = GetMousePosition();

    mousePos = (Vector2){
        mousePos.x - CURSOR_SPRITE_SCALE,
        mousePos.y - CURSOR_SPRITE_SCALE}; // the cursors have an empty pixel in the top left

    DrawTextureV(currentCursorSprite, mousePos, WHITE);
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

void CleanupCursors()
{
    UnloadTexture(pointerSprite);
    UnloadTexture(handSprite);
}

Vector2 GetMouseWorldPosition()
{
    return mouseWorldPos;
}

void SetCurrentCursorSprite(std::string cursor)
{
    if (cursor == "POINTER")
    {
        currentCursorSprite = pointerSprite;
    }
    else if (cursor == "HAND")
    {
        currentCursorSprite = handSprite;
    }
    else
    {
        currentCursorSprite = pointerSprite;
    }
}