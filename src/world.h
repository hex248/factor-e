#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"

#define MAP_TILE_SIZE 100
#define MAP_SIZE_X 13
#define MAP_SIZE_Y 13

typedef struct Tile
{
    unsigned char id;
    char name[16];
    Color color;
} Tile;

typedef struct Map
{
    unsigned int tilesX;
    unsigned int tilesY;
    unsigned char *tileIds;
    Tile *tiles;
} Map;

void InitWorld(Map *map);
void DrawWorld(Map *map);
void CleanupWorld(Map *map);

#endif