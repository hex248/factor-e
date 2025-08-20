#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_SPRITE_PATH "assets/sprites/player.png"
#define PLAYER_SPRITE_SCALE 3.0f
#define PLAYER_SPEED 25
#define PLAYER_REACH 3 // player reach (in tiles)

class Player
{
private:
    Texture2D sprite;
    bool spriteLoaded;

public:
    Vector2 position;
    Vector2 dir = {0, 0};
    float size;
    float speed = 20.0f;
    Color color;

    Player();
    Player(Vector2 pos, float sz, float sp, Color col);
    ~Player();
    void Initialize(Vector2 pos, float sz, float sp, Color col);
    void Draw();
    void HandleMovement();
    void LoadSprite();
    void UnloadSprite();
};

extern Player player;

#endif