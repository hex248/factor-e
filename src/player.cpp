#include "player.h"
#include <cmath>
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
int angleOff = 0;
bool stationary = true;
void Player::Draw()
{
    angleOff ++;
    if (!spriteLoaded)
        return;
    float angle = std::atan2(dir.x, dir.y) * 180 /  PI;

    Rectangle source = {0.0f, 0.0f, (float)sprite.width, float(sprite.height)};

    Rectangle pos = {position.x, position.y, sprite.width *1.0f, sprite.height * 1.0f};

    Vector2 origin = {pos.width / 2.0f, pos.height /2.0f};

    float angleOut;

    if (!stationary) {
        angleOff = 0;
    }
    
    if (dir.x == 0 && dir.y == 0 && ! stationary) {
        stationary = true;
    }
    if (dir.x == 0 && dir.y == 0 && stationary) {
        angleOut = angleOff;
    }else {
        angleOut = angle;
    }
    if (dir.x != 0 && dir.y != 0) {
        stationary = false;
    }
    
    DrawTexturePro(sprite, source, pos, origin, angleOut, color);

}

void Player::HandleMovement()
{
    dir.x =0;
    dir.y =0;
    
    if (IsKeyDown(KEY_W))
        dir.y += 1;
    if (IsKeyDown(KEY_S))
        dir.y -= 1;
    if (IsKeyDown(KEY_A))
        dir.x -= 1;
    if (IsKeyDown(KEY_D))
        dir.x += 1;
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        dir.x = 0;
    }
    if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) {
        dir.y = 0;
    }
    
    position.x += speed * 50.0f * GetFrameTime() * dir.x;
    position.y += speed * 50.0f * GetFrameTime() * dir.y *-1;
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
