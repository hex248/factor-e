#include "world.h"
#include "config.h"
#include "ui.h"
#include <stdlib.h>
#include <stdio.h>

void InitWorld(Map *map)
{
    map->tilesX = MAP_SIZE_X;
    map->tilesY = MAP_SIZE_Y;

    map->tileIds = (unsigned char *)calloc(map->tilesX * map->tilesY, sizeof(unsigned char));
    for (unsigned int i = 0; i < map->tilesY * map->tilesX; i++)
        map->tileIds[i] = i;

    map->tiles = (Tile *)calloc(map->tilesX * map->tilesY, sizeof(Tile));
    for (unsigned int i = 0; i < map->tilesY * map->tilesX; i++)
    {
        map->tiles[i].id = i;
        snprintf(map->tiles[i].name, sizeof(map->tiles[i].name), "%d", i + 1);
        int lightness = GetRandomValue(0, 120);
        map->tiles[i].color = (Color){lightness, lightness + 80, lightness, 255};
    }
}

void DrawWorld(Map *map)
{
    // position grid at a fixed world coordinate (centered around 0,0)
    float offsetX = -(MAP_TILE_SIZE * MAP_SIZE_X) / 2.0f;
    float offsetY = -(MAP_TILE_SIZE * MAP_SIZE_Y) / 2.0f;

    Font fontSmall = GetFontSmall();

    // draw tiles
    for (unsigned int y = 0; y < map->tilesY; y++)
    {
        for (unsigned int x = 0; x < map->tilesX; x++)
        {
            int tileX = (int)(offsetX + x * MAP_TILE_SIZE);
            int tileY = (int)(offsetY + y * MAP_TILE_SIZE);

            DrawRectangle(tileX, tileY, MAP_TILE_SIZE, MAP_TILE_SIZE, map->tiles[y * map->tilesX + x].color);

            Vector2 textSize = MeasureTextEx(fontSmall, map->tiles[y * map->tilesX + x].name, (float)fontSmall.baseSize, 2);
            Vector2 textPos = {
                tileX + (MAP_TILE_SIZE - textSize.x) / 2,
                tileY + (MAP_TILE_SIZE - textSize.y) / 2};
            DrawTextEx(fontSmall, map->tiles[y * map->tilesX + x].name, textPos, (float)fontSmall.baseSize, 2, WHITE);
        }
    }

    // draw grid lines
    int gridStartX = (int)offsetX;
    int gridStartY = (int)offsetY;
    int gridEndX = gridStartX + MAP_SIZE_X * MAP_TILE_SIZE;
    int gridEndY = gridStartY + MAP_SIZE_Y * MAP_TILE_SIZE;

    for (unsigned int x = 0; x <= MAP_SIZE_X; x++)
    {
        int lineX = gridStartX + x * MAP_TILE_SIZE;
        DrawLine(lineX, gridStartY, lineX, gridEndY, WHITE);
    }

    for (unsigned int y = 0; y <= MAP_SIZE_Y; y++)
    {
        int lineY = gridStartY + y * MAP_TILE_SIZE;
        DrawLine(gridStartX, lineY, gridEndX, lineY, WHITE);
    }
}

void CleanupWorld(Map *map)
{
    if (map->tileIds)
    {
        free(map->tileIds);
        map->tileIds = NULL;
    }
    if (map->tiles)
    {
        free(map->tiles);
        map->tiles = NULL;
    }
}
