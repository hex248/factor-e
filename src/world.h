#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <string>
#include <vector>
#include <map>

class Player;

#define TILE_HOVER_SPRITE_PATH "assets/sprites/selection.png"
#define TILE_HOVER_SPRITE_SCALE 4.0f

#define TILE_TYPES_PATH "assets/data/tiles/tile_types.json"
#define TILE_SETS_PATH "assets/data/tiles/tile_sets.json"

#define MAP_TILE_SIZE 128
#define MAP_SIZE_X 16
#define MAP_SIZE_Y 9

typedef struct WorldTile
{
    unsigned char id;
    char name[16];
    Color color;
    Texture2D sprite;
    Rectangle bounds;
    bool hovered;
    char cursorType[8];
    bool useShader;
    char largeTexturePath[256];
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
    char cursorType[8];
    bool useShader;
    std::string largeTexturePath;
} TileType;

/*
{
    "id": 0,
    "name": "Field",
    "description": "very green place",
    "tiles": ["grass", "dirt", "tree", "rock", "poison_flower"],
    "weights": {
        "grass": 80,
        "dirt": 20,
        "tree": 20,
        "rock": 5,
        "poison_flower": 5
    },
    "rules": {
        "tree": {
            "CANNOT_BE_ADJACENT_TO": ["tree", "rock"],
            "MUST_BE_ON": ["grass", "dirt"]
        },
        "rock": {
            "CANNOT_BE_ADJACENT_TO": ["rock", "tree"],
            "MUST_BE_ON": ["grass", "dirt"]
        }
    }
}
*/
enum class Rule
{
    CANNOT_BE_ADJACENT_TO,
    MUST_BE_ON
};

typedef struct TileSet
{
    int id;
    std::string name;
    std::string description;
    std::map<std::string, float> weights;
    std::map<
        std::string,
        std::map<
            Rule,
            std::vector<std::string>>>
        rules;
} TileSet;

typedef struct World
{
    int tilesX;
    int tilesY;
    unsigned char *tileIds;
    WorldTile *tiles;
} World;

typedef struct LgTexShaderData
{
    Shader shader;
    std::map<std::string, Texture2D> textures;
    int tileSizeLoc;
    int textureSizeLoc;
    int tilesInTextureLoc;
    bool initialized;
} LgTexShaderData;

void InitWorld(World *world);
void InitTextureShader();
void CleanupTextureShader();
void DrawWorld(World *world);
void CleanupWorld(World *world);
void CheckHover(World *world);

#endif