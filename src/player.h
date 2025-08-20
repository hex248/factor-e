#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#define PLAYER_SPRITE_PATH "assets/sprites/player.png"
#define PLAYER_SPRITE_SCALE 3.0f
#define PLAYER_SPEED 8
#define PLAYER_REACH 3 // player reach (in tiles)

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

Player *CreateGlobalPlayer(Vector2 pos, float sz, float sp, Color col);
Player *GetGlobalPlayer();

#endif