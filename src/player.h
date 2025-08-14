#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_SPEED 5

class Player
{
public:
    Vector2 position;
    float size;
    float speed = 5.0f;
    Color color;

    Player(Vector2 pos, float sz, float sp, Color col);
    void Draw();
    void HandleMovement();
};

#endif