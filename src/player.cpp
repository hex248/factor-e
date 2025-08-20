#include "player.h"

static Player *globalPlayer = nullptr;

Player::Player(Vector2 pos, float sz, float sp, Color col) : position(pos), size(sz), speed(sp), color(col), spriteLoaded(false)
{
    LoadSprite();
}

Player::~Player()
{
    UnloadSprite();
}

void Player::LoadSprite()
{
    if (spriteLoaded)
        return;

    Image playerImage = LoadImage(PLAYER_SPRITE_PATH);

    // nearest neighbour resize sprite
    ImageResizeNN(&playerImage,
                  (int)(playerImage.width * PLAYER_SPRITE_SCALE),
                  (int)(playerImage.height * PLAYER_SPRITE_SCALE));

    sprite = LoadTextureFromImage(playerImage);
    UnloadImage(playerImage);
    spriteLoaded = true;
}

void Player::UnloadSprite()
{
    if (spriteLoaded)
    {
        UnloadTexture(sprite);
        spriteLoaded = false;
    }
}

void Player::Draw()
{
    if (!spriteLoaded)
        return;

    DrawTexture(sprite,
                (int)(position.x - sprite.width / 2.0f),
                (int)(position.y - sprite.height / 2.0f),
                color);
}

void Player::HandleMovement()
{
    if (IsKeyDown(KEY_W))
        position.y -= speed * 50.0f * GetFrameTime();
    if (IsKeyDown(KEY_S))
        position.y += speed * 50.0f * GetFrameTime();
    if (IsKeyDown(KEY_A))
        position.x -= speed * 50.0f * GetFrameTime();
    if (IsKeyDown(KEY_D))
        position.x += speed * 50.0f * GetFrameTime();
}

Player *CreateGlobalPlayer(Vector2 pos, float sz, float sp, Color col)
{
    globalPlayer = new Player(pos, sz, sp, col);
    return globalPlayer;
}

Player *GetGlobalPlayer()
{
    return globalPlayer;
}
