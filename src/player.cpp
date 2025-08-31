#include "player.h"
#include "tex.h"
#include <cmath>

Player player;

Player::Player() : position({0.0f, 0.0f}), size(25.0f), speed(PLAYER_SPEED), spriteLoaded(false)
{
}

Player::Player(Vector2 pos, float sz, float sp) : position(pos), size(sz), speed(sp), spriteLoaded(false)
{
}

Player::~Player()
{
}

void Player::Initialize(Vector2 pos, float sz, float sp)
{
    inventory[0] = CreateItemStack(1, 128);
    inventory[1] = CreateItemStack(2, 16);
    position = pos;
    size = sz;
    speed = sp;
    LoadSprite();
}

void Player::LoadSprite()
{
    if (spriteLoaded)
        return;

    Image playerImage = LoadImage(PLAYER_SPRITE_PATH);

    // nearest neighbour resize sprite
    ImageResizeNN(&playerImage,
                  (int)((float)(playerImage.width) * PLAYER_SPRITE_SCALE),
                  (int)((float)(playerImage.height) * PLAYER_SPRITE_SCALE));

    sprite = RegisterTexture(playerImage);

    Image playerShadowImage = LoadImage(PLAYER_SHADOW_SPRITE_PATH);

    ImageResizeNN(&playerShadowImage,
                  (int)((float)(playerShadowImage.width) * PLAYER_SPRITE_SCALE),
                  (int)((float)(playerShadowImage.height) * PLAYER_SPRITE_SCALE));

    shadowSprite = RegisterTexture(playerShadowImage);

    UnloadImage(playerImage);
    UnloadImage(playerShadowImage);

    spriteLoaded = true;
}

float angleOff = 0;
bool stationary = true;
void Player::Draw()
{
    if (!spriteLoaded)
        return;
    float angle = std::atan2(dir.x, dir.y) * 180 / PI;

    Texture2D playerTex = GetTexture(sprite);
    Texture2D shadowTex = GetTexture(shadowSprite);

    Rectangle source = {0.0f, 0.0f, (float)(playerTex.width), (float)(playerTex.height)};

    Vector2 shadowPos = {position.x - shadowTex.width / 2.0f, (position.y - shadowTex.height / 2.0f) + PLAYER_SPRITE_SCALE};
    Rectangle pos = {position.x, position.y, (float)(playerTex.width), (float)(playerTex.height)};

    Vector2 origin = {pos.width / 2.0f, pos.height / 2.0f};

    DrawTextureV(shadowTex, shadowPos, WHITE);
    DrawTexturePro(playerTex, source, pos, origin, 0, WHITE);

    if (selectedSlot < 7 && inventory[selectedSlot].quantity > 0)
    {
        Texture2D inHandSprite = GetTexture(inventory[selectedSlot].inHandSprite);
        Vector2 inHandSpritePos = {(position.x - inHandSprite.width / 2) + inventory[selectedSlot].inHandOffset.x,
                                   (position.y - inHandSprite.height / 2) + inventory[selectedSlot].inHandOffset.y};

        DrawTextureV(inHandSprite, inHandSpritePos, WHITE);
    }
}

void Player::HandleMovement()
{
    dir.x = 0;
    dir.y = 0;

    if (IsKeyDown(KEY_W))
        dir.y += 1;
    if (IsKeyDown(KEY_S))
        dir.y -= 1;
    if (IsKeyDown(KEY_A))
        dir.x -= 1;
    if (IsKeyDown(KEY_D))
        dir.x += 1;
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))
    {
        dir.x = 0;
    }
    if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
    {
        dir.y = 0;
    }

    position.x += speed * 50.0f * GetFrameTime() * dir.x;
    position.y += speed * 50.0f * GetFrameTime() * dir.y * -1;
}
