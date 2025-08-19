#ifndef MOUSE_H
#define MOUSE_H

#include "raylib.h"

#define POINTER_SPRITE_PATH "assets/sprites/cursors/pointer.png"
#define HAND_SPRITE_PATH "assets/sprites/cursors/hand.png"
#define CURSOR_SPRITE_SCALE 3.0f

void InitialiseMouse();
void HandleMouse(Camera2D camera);
void DrawMouse();
void HideMouse();
void ShowMouse();

#endif