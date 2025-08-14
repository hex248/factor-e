#include "player.h"

Player::Player(Vector2 pos, float sz, float sp, Color col) : position(pos), size(sz), speed(sp), color(col) {}

void Player::Draw()
{
    DrawCircleV(position, size, color);
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
