#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <string>
#include <vector>
#include <map>
#include "include/json.hpp"

class Player;

typedef struct Diamond
{
    Vector2 center;
    float width;
    float height;
} Diamond;

#define TILE_HOVER_SPRITE_PATH "assets/sprites/tiles/selection.png"
#define TILE_HOVER_SPRITE_SCALE 8.0f

#define TILE_TYPES_PATH "assets/data/tiles/tile_types.json"
#define TILE_SETS_PATH "assets/data/tiles/tile_sets.json"

#define WORLD_FILE_NAME "world.bin"
#define WORLD_TILE_SIZE 256
#define WORLD_SIZE_X 15
#define WORLD_SIZE_Y 15

typedef struct WorldTile
{
    unsigned char id;
    char name[16];
    Texture2D sprite;
    Diamond bounds;
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

void to_json(nlohmann::json &j, const CollisionType &c);
void from_json(const nlohmann::json &j, CollisionType &c);
void to_json(nlohmann::json &j, const TileType &t);
void from_json(const nlohmann::json &j, TileType &t);
void to_json(nlohmann::json &j, const TileSet &t);
void from_json(const nlohmann::json &j, TileSet &t);

// World save/load functions
int World_Save(const char *path, struct World *world);
int World_Load(const char *path, struct World *world);
void RegenerateTexturesForLoadedWorld(World *world);

TileType GetRandomWeightedTile(const std::map<std::string, float> &weights);
TileType GetTileFromNoiseWeighted(int x, int y, Image noiseImage, const std::map<std::string, float> &weights);

Texture2D GetOrLoadShaderTexture(const std::string &texturePath);

void GenerateWorld(World *world);
void InitWorld(World *world);
void InitTextureShader();
void CleanupTextureShader();
void DrawWorld(World *world);
void CleanupWorld(World *world);
void CheckHover(World *world);
bool CheckPointInDiamond(Vector2 point, Diamond diamond);

#endif