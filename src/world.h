#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <string>

#define TILE_TYPES_PATH "assets/data/tiles/tile_types.json"
#define TILE_SETS_PATH "assets/data/tiles/tile_sets.json"

#define MAP_TILE_SIZE 128
#define MAP_SIZE_X 9
#define MAP_SIZE_Y 9

typedef struct WorldTile
{
    unsigned char id;
    char name[16];
    Color color;
    Texture2D sprite;
} WorldTile;

typedef struct CollisionType
{
    bool solid;
    float width;
    float height;
} CollisionType;

/*
{
    "id": 0,
    "name": "Dirt",
    "description": "dirt",
    "layer": "ground",
    "spritePath": "assets/sprites/dirt.png",
    "spriteScale": 4.0,
    "collision": {
        "solid": false,
        "width": 1,
        "height": 1
    },
    "walkable": true,
    "buildable": true,
    "harvestable": false
}
*/
typedef struct TileType
{
    int id;
    std::string name;
    std::string description;
    std::string layer;
    std::string spritePath;
    float spriteScale;
    CollisionType collision;
} TileType;

typedef struct Map
{
    unsigned int tilesX;
    unsigned int tilesY;
    unsigned char *tileIds;
    WorldTile *tiles;
} Map;

void InitWorld(Map *map);
void DrawWorld(Map *map);
void CleanupWorld(Map *map);

#endif