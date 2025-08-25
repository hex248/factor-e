#include "mouse.h"
#include "config.h"
#include <cstring>
#include <stdio.h>

Texture2D pointerSprite;
Texture2D handSprite;
Texture2D currentCursorSprite;

static bool showMouse = false;
static bool showCustomCursor = false;

static Vector2 mouseScreenPos = {0, 0};
static Vector2 mouseWorldPos = {0, 0};

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
                  (int)((float)pointerImage.width * CURSOR_SPRITE_SCALE),
                  (int)((float)pointerImage.height * CURSOR_SPRITE_SCALE));
    ImageResizeNN(&handImage,
                  (int)((float)handImage.width * CURSOR_SPRITE_SCALE),
                  (int)((float)handImage.height * CURSOR_SPRITE_SCALE));

    pointerSprite = LoadTextureFromImage(pointerImage);
    UnloadImage(pointerImage);
    handSprite = LoadTextureFromImage(handImage);
    UnloadImage(handImage);
}

void HandleMouse(Camera2D camera)
{
    mouseScreenPos = GetMouseVirtualPosition();
    mouseWorldPos = GetScreenToWorld2D(mouseScreenPos, camera);
}

void DrawMouse()
{
    if (!showCustomCursor)
        return;
    Vector2 mousePos = GetMouseVirtualPosition();

    mousePos = {
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

Vector2 GetMouseVirtualPosition()
{
    Vector2 screenPos = GetMousePosition();

    // transform screen coordinates to virtual screen coordinates
    Vector2 virtualPos;
    virtualPos.x = (screenPos.x - targetRect.x) / virtualScale.x;
    virtualPos.y = (screenPos.y - targetRect.y) / virtualScale.y;

    if (virtualPos.x < 0)
        virtualPos.x = 0;
    if (virtualPos.y < 0)
        virtualPos.y = 0;
    if (virtualPos.x > VIRTUAL_WIDTH)
        virtualPos.x = VIRTUAL_WIDTH;
    if (virtualPos.y > VIRTUAL_HEIGHT)
        virtualPos.y = VIRTUAL_HEIGHT;

    return virtualPos;
}

void SetCurrentCursorSprite(const char *cursor)
{
    if (strcmp(cursor, "POINTER") == 0)
    {
        currentCursorSprite = pointerSprite;
    }
    else if (strcmp(cursor, "HAND") == 0)
    {
        currentCursorSprite = handSprite;
    }
    else
    {
        currentCursorSprite = pointerSprite;
    }
}