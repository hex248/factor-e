#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "item.h"

#define PLAYER_SPRITE_PATH "assets/sprites/player.png"
#define PLAYER_SPRITE_SCALE 3.0f
#define PLAYER_SPEED 25
#define PLAYER_REACH 4

class Player
{
private:
    Texture2D sprite;

public:
    ItemStack inventory[21];
    unsigned int selectedSlot = 7;

    Vector2 position = {0, 0};
    Vector2 dir = {0, 0};
    float size = 1.0f;
    float speed = 20.0f;
    Color color = WHITE;
    bool spriteLoaded = false;

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