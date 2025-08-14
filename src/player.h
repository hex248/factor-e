#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_SPRITE_PATH "assets/sprites/star1.png"
#define PLAYER_SPRITE_SCALE 2.0f
#define PLAYER_SPEED 5

class Player
{
private:
    Texture2D sprite;
    bool spriteLoaded;

public:
    Vector2 position;
    float size;
    float speed = 5.0f;
    Color color;

    Player(Vector2 pos, float sz, float sp, Color col);
    ~Player();
    void Draw();
    void HandleMovement();
    void LoadSprite();
    void UnloadSprite();
};

#endif