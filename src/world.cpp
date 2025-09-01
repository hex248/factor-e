#include "world.h"
#include "config.h"
#include "ui.h"
#include "mouse.h"
#include "player.h"
#include "controls.h"
#include "tex.h"
#include "include/json.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

#include <stdint.h>
#include <memory.h>
#include <string.h>

World world;

const char *missingTileSprite = "assets/sprites/tiles/missing.png";

float missingTileScale = 8.0f;

WorldTile emptyTile = {
    .id = 0,
    .name = "Empty",
    .sprite = "",
    .bounds = {0, 0, 0, 0},
    .hovered = false,
    .cursorType = "HAND",
    .useShader = false,
    .largeTexturePath = ""};

Image noise;
Texture2D noiseTexture;

unsigned char tileHoverSprite;
unsigned char potentialTileHoverSprite;
static LgTexShaderData lgTexShader;

using json = nlohmann::json;

// json parsing and serialization functions for each type
void to_json(json &j, const TileType &t)
{
    j = json{{"name", t.name},
             {"description", t.description},
             {"layer", t.layer},
             {"spritePath", t.spritePath},
             {"spriteScale", t.spriteScale},
             {"cursorType", std::string(t.cursorType)},
             {"useShader", t.useShader},
             {"largeTexturePath", t.largeTexturePath}};
}

void from_json(const json &j, TileType &t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("description").get_to(t.description);
    j.at("layer").get_to(t.layer);
    j.at("spritePath").get_to(t.spritePath);
    j.at("spriteScale").get_to(t.spriteScale);
    std::string cursorTypeStr;
    j.at("cursorType").get_to(cursorTypeStr);
    strncpy(t.cursorType, cursorTypeStr.c_str(), sizeof(t.cursorType) - 1);
    t.cursorType[sizeof(t.cursorType) - 1] = '\0';

    if (j.contains("useShader"))
        j.at("useShader").get_to(t.useShader);
    else
        t.useShader = false;

    if (j.contains("largeTexturePath"))
        j.at("largeTexturePath").get_to(t.largeTexturePath);
    else
        t.largeTexturePath = "";
}

void to_json(json &j, const TileSet &t)
{
    j = json{{"id", t.id},
             {"name", t.name},
             {"description", t.description},
             {"weights", t.weights},
             {"rules", t.rules}};
}

void from_json(const json &j, TileSet &t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("description").get_to(t.description);

    for (const auto &item : j.at("weights").items())
    {
        t.weights[item.key()] = item.value().get<float>();
    }

    /*
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
    */
    for (const auto &ruleItem : j.at("rules").items())
    {
        std::string tileName = ruleItem.key();

        for (const auto &ruleType : ruleItem.value().items())
        {
            Rule rule;
            std::string ruleTypeStr = ruleType.key();
            if (ruleTypeStr == "CANNOT_BE_ADJACENT_TO")
            {
                rule = Rule::CANNOT_BE_ADJACENT_TO;
            }
            else if (ruleTypeStr == "MUST_BE_ON")
            {
                rule = Rule::MUST_BE_ON;
            }
            else
                continue;

            std::vector<std::string> tiles;
            for (const auto &tile : ruleType.value())
            {
                std::string tileStr = tile.get<std::string>();
                tiles.push_back(tileStr);
            }

            t.rules[tileName][rule] = tiles;
        }
    }
}

std::ifstream tile_types_json(TILE_TYPES_PATH);
json tile_types_data = json::parse(tile_types_json);

TileType GetTileByID(unsigned char tileID)
{
    for (const auto &item : tile_types_data.items())
    {
        TileType tileType = item.value().get<TileType>();
        if (tileType.id == tileID)
        {
            return tileType;
        }
    }
    return {};
}

// get tile set data from json file
std::ifstream tile_sets_json(TILE_SETS_PATH);
json tile_sets_data = json::parse(tile_sets_json);

TileSet GetTileSetByID(unsigned char tileSetID)
{
    for (const auto &item : tile_sets_data.items())
    {
        TileSet tileSet = item.value().get<TileSet>();
        if (tileSet.id == tileSetID)
        {
            return tileSet;
        }
    }
    return {};
}

void RegenerateTexturesForLoadedWorld()
{
    for (int i = 0; i < world.tilesX * world.tilesY; i++)
    {
        if (!world.tiles[i].useShader && strlen(world.tiles[i].name) > 0)
        {
            std::string tileName = world.tiles[i].name;

            // convert to lowercase
            std::transform(tileName.begin(), tileName.end(), tileName.begin(), ::tolower);

            for (auto &[key, value] : tile_types_data.items())
            {
                if (key == tileName)
                {
                    TileType tileType = value.template get<TileType>();

                    Texture2D tileTex = GetTexture(tileType.spritePath);

                    if (tileTex.id > 0)
                    {
                        strncpy(world.tiles[i].sprite, tileType.spritePath.c_str(), sizeof(world.tiles[i].sprite) - 1);
                        world.tiles[i].sprite[sizeof(world.tiles[i].sprite) - 1] = '\0';
                    }
                    else
                    {
                        Image spriteImage = LoadImage(tileType.spritePath.c_str());
                        if (spriteImage.data != NULL)
                        {
                            ImageResizeNN(&spriteImage,
                                          (int)((float)spriteImage.width * tileType.spriteScale),
                                          (int)((float)spriteImage.height * tileType.spriteScale));

                            std::string textureKey = RegisterTexture(spriteImage, tileType.spritePath);
                            strncpy(world.tiles[i].sprite, textureKey.c_str(), sizeof(world.tiles[i].sprite) - 1);
                            world.tiles[i].sprite[sizeof(world.tiles[i].sprite) - 1] = '\0';
                            UnloadImage(spriteImage);
                        }
                    }

                    break;
                }
            }
        }
    }
}

int World_Save(const char *path)
{
    FILE *fp;
    size_t written;

    fp = fopen(path, "wb");
    if (fp == NULL)
    {
        printf("WORLD SAVE: failed to fopen file \"%s\"", path);
        return 0;
    }

    // save the world struct (tilesX, tilesY, but tiles pointer will be recreated)
    written = fwrite(&world.tilesX, sizeof(world.tilesX), 1, fp);
    if (written != 1)
    {
        printf("WORLD SAVE: failed to write tilesX to \"%s\"", path);
        fclose(fp);
        return 0;
    }

    written = fwrite(&world.tilesY, sizeof(world.tilesY), 1, fp);
    if (written != 1)
    {
        printf("WORLD SAVE: failed to write tilesY to \"%s\"", path);
        fclose(fp);
        return 0;
    }

    // save the tiles array data
    if (world.tiles != NULL)
    {
        size_t tilesCount = (size_t)(world.tilesX * world.tilesY);
        written = fwrite(world.tiles, sizeof(WorldTile), tilesCount, fp);
        if (written != tilesCount)
        {
            printf("WORLD SAVE: failed to write tiles data to \"%s\"", path);
            fclose(fp);
            return 0;
        }
    }

    fflush(fp);
    fclose(fp);
    return 1;
}

int World_Load(const char *path)
{
    FILE *fp;
    size_t num_read;

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("WORLD LOAD: failed to fopen file \"%s\"", path);
        return 0;
    }

    num_read = fread(&world.tilesX, sizeof(world.tilesX), 1, fp);
    if (num_read != 1)
    {
        printf("WORLD LOAD: failed to read tilesX from \"%s\"", path);
        fclose(fp);
        return 0;
    }

    num_read = fread(&world.tilesY, sizeof(world.tilesY), 1, fp);
    if (num_read != 1)
    {
        printf("WORLD LOAD: failed to read tilesY from \"%s\"", path);
        fclose(fp);
        return 0;
    }

    size_t tilesCount = (size_t)(world.tilesX * world.tilesY);
    world.tiles = (WorldTile *)calloc(tilesCount, sizeof(WorldTile));
    if (world.tiles == NULL)
    {
        printf("WORLD LOAD: failed to allocate memory for tiles");
        fclose(fp);
        return 0;
    }

    num_read = fread(world.tiles, sizeof(WorldTile), tilesCount, fp);
    if (num_read != tilesCount)
    {
        printf("WORLD LOAD: failed to read tiles data from \"%s\"", path);
        free(world.tiles);
        world.tiles = NULL;
        fclose(fp);
        return 0;
    }

    fclose(fp);

    InitTextureShader();

    RegenerateTexturesForLoadedWorld();

    return 1;
}

TileType GetRandomWeightedTile(const std::map<std::string, float> &weights)
{
    float totalWeight = 0;
    for (const auto &[key, value] : weights)
    {
        totalWeight += value;
    }

    float randomValue = (float)GetRandomValue(0, (int)totalWeight - 1);

    // ?sort weights descending
    // this ensures that all weights are accounted for, even if the second weight is greater than the first
    // EXAMPLE:
    //     grass: 20
    //     dirt: 40
    // dirt has 40/60 chance and grass has 20/60
    // without sorting, grass would have been chosen much more frequently as it comes first.
    std::vector<std::pair<std::string, float>> sortedWeights;
    for (const auto &[key, value] : weights)
    {
        sortedWeights.emplace_back(key, value);
    }
    std::sort(sortedWeights.begin(), sortedWeights.end(),
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    for (const auto &[key, value] : sortedWeights)
    {
        if (randomValue < value)
        {
            return tile_types_data[key].template get<TileType>();
        }
        randomValue -= value;
    }

    return {};
}

TileType GetTileFromNoiseWeighted(int x, int y, Image noiseImage, const std::map<std::string, float> &weights)
{
    float totalWeight = 0;
    for (const auto &[key, value] : weights)
    {
        totalWeight += value;
    }

    // get pixel color from noise image
    Color pixelColor = GetImageColor(noiseImage, x * WORLD_TILE_SIZE, y * WORLD_TILE_SIZE);

    // normalize lightness to totalWeight
    float lightness = ((float)pixelColor.r / 255) * totalWeight;

    // use weight to choose a tile based on lightness
    for (const auto &[key, value] : weights)
    {
        if (lightness < value)
        {
            return tile_types_data[key].template get<TileType>();
        }
        lightness -= value;
    }

    return GetRandomWeightedTile(weights);
}

void InitTextureShader()
{
    if (lgTexShader.initialized)
        return;

    lgTexShader.shader = LoadShader("assets/shaders/large_texture.vs", "assets/shaders/large_texture.fs");

    lgTexShader.tileSizeLoc = GetShaderLocation(lgTexShader.shader, "tileSize");
    lgTexShader.textureSizeLoc = GetShaderLocation(lgTexShader.shader, "textureSize");
    lgTexShader.tilesInTextureLoc = GetShaderLocation(lgTexShader.shader, "tilesInTexture");

    lgTexShader.initialized = true;
}

void CleanupTextureShader()
{
    if (lgTexShader.initialized)
    {
        UnloadShader(lgTexShader.shader);

        // unload all cached textures
        for (auto &[path, texture] : lgTexShader.textures)
        {
            UnloadTexture(texture);
        }
        lgTexShader.textures.clear();

        lgTexShader.initialized = false;
    }
}

Texture2D GetOrLoadShaderTexture(const std::string &texturePath)
{
    auto it = lgTexShader.textures.find(texturePath);
    if (it != lgTexShader.textures.end())
    {
        return it->second;
    }

    // load and cache texture
    Image image = LoadImage(texturePath.c_str());
    ImageResizeNN(&image,
                  (int)(image.width * 8),
                  (int)(image.height * 8));

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    lgTexShader.textures[texturePath] = texture;
    return texture;
}

void GenerateWorld()
{
    // generate a tiny perlin noise image (using world size, so that it is 1 pixel per tile)
    int seedX = GetRandomValue(0, 10000000);
    int seedY = GetRandomValue(0, 10000000);
    noise = GenImagePerlinNoise(WORLD_SIZE_X, WORLD_SIZE_Y, seedX, seedY, 1.5);
    ImageResizeNN(&noise, WORLD_SIZE_X * WORLD_TILE_SIZE, WORLD_SIZE_Y * WORLD_TILE_SIZE);
    noiseTexture = LoadTextureFromImage(noise);

    InitTextureShader();

    TileSet tile_set = tile_sets_data["field_biome"].template get<TileSet>();

    world.tilesX = WORLD_SIZE_X;
    world.tilesY = WORLD_SIZE_Y;

    // TODO: implement multiple layers

    std::vector<std::string> layers = {"ground"};

    world.tiles = (WorldTile *)calloc(WORLD_SIZE_X * WORLD_SIZE_Y, sizeof(WorldTile));
    for (int y = 0; y < WORLD_SIZE_Y; y++)
    {
        for (int x = 0; x < WORLD_SIZE_X; x++)
        {
            int i = y * WORLD_SIZE_X + x;

            world.tiles[i].id = (unsigned char)i;

            TileType tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            // TODO: IMPLEMENT RULES
            // if the tile's layer is not in the layers vector, get a new tile
            while (std::find(layers.begin(), layers.end(), tile.layer) == layers.end())
            {
                tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            }
            snprintf(world.tiles[i].name, sizeof(world.tiles[i].name), "%s", tile.name.c_str());

            if (!tile.useShader)
            {
                Texture2D tileTex = GetTexture(tile.spritePath);

                if (tileTex.id > 0)
                {
                    strncpy(world.tiles[i].sprite, tile.spritePath.c_str(), sizeof(world.tiles[i].sprite) - 1);
                    world.tiles[i].sprite[sizeof(world.tiles[i].sprite) - 1] = '\0';
                }
                else
                {
                    Image spriteImage = LoadImage(tile.spritePath.c_str());

                    ImageResizeNN(&spriteImage,
                                  (int)((float)spriteImage.width * tile.spriteScale),
                                  (int)((float)spriteImage.height * tile.spriteScale));

                    std::string textureKey = RegisterTexture(spriteImage, tile.spritePath);
                    strncpy(world.tiles[i].sprite, textureKey.c_str(), sizeof(world.tiles[i].sprite) - 1);
                    world.tiles[i].sprite[sizeof(world.tiles[i].sprite) - 1] = '\0';
                    UnloadImage(spriteImage);
                }
            }
            snprintf(world.tiles[i].cursorType, sizeof(world.tiles[i].cursorType), "%s", tile.cursorType);
            world.tiles[i].useShader = tile.useShader;
            snprintf(world.tiles[i].largeTexturePath, sizeof(world.tiles[i].largeTexturePath), "%s", tile.largeTexturePath.c_str());
        }
    }

    World_Save(WORLD_FILE_NAME);
}

void InitWorld()
{
    Image missingTileImage = LoadImage(missingTileSprite);

    ImageResizeNN(&missingTileImage,
                  (int)((float)missingTileImage.width * missingTileScale),
                  (int)((float)missingTileImage.height * missingTileScale));
    RegisterTexture(missingTileImage, missingTileSprite);
    UnloadImage(missingTileImage);

    Image tileHoverImage = LoadImage(TILE_HOVER_SPRITE_PATH);

    ImageResizeNN(&tileHoverImage,
                  (int)((float)tileHoverImage.width * TILE_HOVER_SPRITE_SCALE),
                  (int)((float)tileHoverImage.height * TILE_HOVER_SPRITE_SCALE));

    tileHoverSprite = RegisterTexture(tileHoverImage);
    UnloadImage(tileHoverImage);

    Image potentialTileHoverImage = LoadImage(POTENTIAL_TILE_HOVER_SPRITE_PATH);
    ImageResizeNN(&potentialTileHoverImage,
                  (int)((float)potentialTileHoverImage.width * POTENTIAL_TILE_HOVER_SPRITE_SCALE),
                  (int)((float)potentialTileHoverImage.height * POTENTIAL_TILE_HOVER_SPRITE_SCALE));
    potentialTileHoverSprite = RegisterTexture(potentialTileHoverImage);
    UnloadImage(potentialTileHoverImage);

    if (FileExists(WORLD_FILE_NAME))
    {
        if (World_Load(WORLD_FILE_NAME) != 1)
        {
            printf("Failed to load world, generating new one\n");
            GenerateWorld();
        }
    }
    else
    {
        GenerateWorld();
    }
}

Vector2 GetIsometricTilePosition(Vector2 in)
{
    Vector2 out;
    out.x = (in.x - in.y) / 2;
    out.y = (in.x + in.y) / 4;

    return out;
}

void DrawWorld()
{
    // position grid at a fixed world coordinate (centered around 0,0)
    float centerGridX = (WORLD_SIZE_X - 1) / 2.0f;
    float centerGridY = (WORLD_SIZE_Y - 1) / 2.0f;
    Vector2 centerIsoPos = GetIsometricTilePosition({centerGridX, centerGridY});

    float centerTileX = centerIsoPos.x * WORLD_TILE_SIZE;
    float centerTileY = centerIsoPos.y * WORLD_TILE_SIZE;

    float tileCenterX = centerTileX + WORLD_TILE_SIZE / 2.0f;
    float tileCenterY = centerTileY + WORLD_TILE_SIZE / 2.0f - WORLD_TILE_SIZE * 0.5f;

    float offsetX = -tileCenterX;
    float offsetY = -tileCenterY;

    // draw tiles
    for (int y = 0; y < world.tilesY; y++)
    {
        for (int x = 0; x < world.tilesX; x++)
        {
            Vector2 isoPos = GetIsometricTilePosition({(float)x, (float)y});

            float tileX = (offsetX + isoPos.x * WORLD_TILE_SIZE);
            float tileY = (offsetY + isoPos.y * WORLD_TILE_SIZE);

            int index = y * world.tilesX + x;

            // if the tile should use a shader and has a largeTexturePath
            if (world.tiles[index].useShader && strlen(world.tiles[index].largeTexturePath) > 0 && lgTexShader.initialized)
            {
                Texture2D lgTex = GetOrLoadShaderTexture(std::string(world.tiles[index].largeTexturePath));

                // if the texture successfully loads
                if (lgTex.id > 0)
                {
                    BeginShaderMode(lgTexShader.shader);

                    // set shader values
                    float tileSize = (float)WORLD_TILE_SIZE;
                    Vector2 textureSize = {
                        (float)lgTex.width,
                        (float)lgTex.height};
                    Vector2 tilesInTexture = {
                        textureSize.x / tileSize,
                        textureSize.y / tileSize};

                    SetShaderValue(lgTexShader.shader, lgTexShader.tileSizeLoc, &tileSize, SHADER_UNIFORM_FLOAT);
                    SetShaderValue(lgTexShader.shader, lgTexShader.textureSizeLoc, &textureSize, SHADER_UNIFORM_VEC2);
                    SetShaderValue(lgTexShader.shader, lgTexShader.tilesInTextureLoc, &tilesInTexture, SHADER_UNIFORM_VEC2);

                    // draw using shader
                    DrawTexturePro(
                        lgTex,
                        {0, 0, (float)lgTex.width, (float)lgTex.height},
                        {tileX, tileY, WORLD_TILE_SIZE, WORLD_TILE_SIZE},
                        {0, 0},
                        0.0f,
                        WHITE);

                    EndShaderMode();
                }
                else
                {
                    // fallback to regular sprite if shader texture failed to load
                    Texture2D sprite = GetTexture(std::string(world.tiles[index].sprite));
                    DrawTexture(sprite, (int)tileX - (sprite.width / 2) + WORLD_TILE_SIZE / 2, (int)tileY - (sprite.height / 2) + WORLD_TILE_SIZE / 2, WHITE);
                }
            }
            // no shader
            else
            {
                Texture2D sprite = GetTexture(std::string(world.tiles[index].sprite));
                if (sprite.id > 0)
                {
                    DrawTexture(sprite, (int)tileX - (sprite.width / 2) + WORLD_TILE_SIZE / 2, (int)tileY - (sprite.height / 2) + WORLD_TILE_SIZE / 2, WHITE);
                }
                else if (strcmp(world.tiles[index].name, emptyTile.name) != 0)
                {
                    printf("Missing tile: %s\n", world.tiles[index].name);
                    printf("Empty tile: %s\n", emptyTile.name);
                    Texture2D missingTex = GetTexture(missingTileSprite);
                    DrawTexture(missingTex, (int)tileX - (missingTex.width / 2) + WORLD_TILE_SIZE / 2, (int)tileY - (missingTex.height / 2) + WORLD_TILE_SIZE / 2, WHITE);
                }

                // set diamond bounds for isometric tile collision (top face)
                world.tiles[index].bounds.center = {(int)tileX + WORLD_TILE_SIZE / 2.0f, (int)tileY + WORLD_TILE_SIZE / 2.0f - WORLD_TILE_SIZE * 0.25f};
                world.tiles[index].bounds.width = WORLD_TILE_SIZE;
                world.tiles[index].bounds.height = WORLD_TILE_SIZE * 0.5f;

                // debug: draw tile bounds
                if (showDebug)
                {
                    Vector2 center = world.tiles[index].bounds.center;
                    float hw = world.tiles[index].bounds.width / 2.0f;
                    float hh = world.tiles[index].bounds.height / 2.0f;

                    Color tileColor = {255, 255, 255, 60};
                    if (std::string(world.tiles[index].name) == std::string(emptyTile.name))
                    {
                        tileColor = {255, 0, 0, 60};
                    }

                    DrawLineEx({center.x - hw, center.y}, {center.x, center.y - hh}, 5, tileColor);
                    DrawLineEx({center.x, center.y - hh}, {center.x + hw, center.y}, 5, tileColor);
                    DrawLineEx({center.x + hw, center.y}, {center.x, center.y + hh}, 5, tileColor);
                    DrawLineEx({center.x, center.y + hh}, {center.x - hw, center.y}, 5, tileColor);
                }
            }
        }
    }
}

bool CheckPointInDiamond(Vector2 point, Diamond diamond)
{
    float relativeX = point.x - diamond.center.x;
    float relativeY = point.y - diamond.center.y;

    float halfWidth = diamond.width / 2.0f;
    float halfHeight = diamond.height / 2.0f;

    float normalisedX = (float)fabs((double)relativeX) / halfWidth;
    float normalisedY = (float)fabs((double)relativeY) / halfHeight;

    return (normalisedX + normalisedY) <= 1.0f;
}

void CheckHover()
{
    Vector2 mouseWorldPos = GetMouseWorldPosition();
    Vector2 mouseScreenPos = GetMouseVirtualPosition();
    WorldTile hoveredTile;
    bool tileFound = false;
    int hoveredIndex = -1;

    float diffX = mouseWorldPos.x - player.position.x;
    float diffY = mouseWorldPos.y - player.position.y;
    float distanceSquared = diffX * diffX + diffY * diffY;

    for (int i = 0; i < world.tilesX * world.tilesY; i++)
    {
        world.tiles[i].hovered = false;
    }

    // debug: draw small circles on tiles within reach
    if (showDebug)
    {
        for (int y = 0; y < world.tilesY; y++)
        {
            for (int x = 0; x < world.tilesX; x++)
            {
                int i = y * world.tilesX + x;

                Vector2 closestPoint;
                float relativeX = player.position.x - world.tiles[i].bounds.center.x;
                float relativeY = player.position.y - world.tiles[i].bounds.center.y;

                float halfWidth = world.tiles[i].bounds.width / 2.0f;
                float halfHeight = world.tiles[i].bounds.height / 2.0f;

                float normalisedX = (float)fabs((double)relativeX) / halfWidth;
                float normalisedY = (float)fabs((double)relativeY) / halfHeight;

                if ((normalisedX + normalisedY) <= 1.0f)
                {
                    closestPoint = player.position;
                }
                else
                {
                    float scale = 1.0f / (normalisedX + normalisedY);
                    closestPoint.x = world.tiles[i].bounds.center.x + relativeX * scale;
                    closestPoint.y = world.tiles[i].bounds.center.y + relativeY * scale;
                }

                float tileDiffX = closestPoint.x - player.position.x;
                float tileDiffY = closestPoint.y - player.position.y;
                float isometricTileDiffY = tileDiffY * 2.0f;
                float tileDistanceSquared = tileDiffX * tileDiffX + isometricTileDiffY * isometricTileDiffY;

                float reach = PLAYER_REACH * WORLD_TILE_SIZE / 2;

                if (tileDistanceSquared < reach * reach)
                {
                    Texture2D potentialHoverTex = GetTexture(potentialTileHoverSprite);
                    DrawTextureV(potentialHoverTex,
                                 {world.tiles[i].bounds.center.x - (float)potentialHoverTex.width / 2,
                                  world.tiles[i].bounds.center.y - (float)potentialHoverTex.height / 2},
                                 WHITE);
                }
            }
        }
    }

    // check tiles from back to front (for isometric)
    for (int y = world.tilesY - 1; y >= 0; y--)
    {
        for (int x = world.tilesX - 1; x >= 0; x--)
        {
            int i = y * world.tilesX + x;

            if (CheckPointInDiamond(mouseWorldPos, world.tiles[i].bounds))
            {
                // find the closest point in the tile diamond to the player
                Vector2 closestPoint;

                float relativeX = player.position.x - world.tiles[i].bounds.center.x;
                float relativeY = player.position.y - world.tiles[i].bounds.center.y;

                float halfWidth = world.tiles[i].bounds.width / 2.0f;
                float halfHeight = world.tiles[i].bounds.height / 2.0f;

                float normalisedX = (float)fabs((double)relativeX) / halfWidth;
                float normalisedY = (float)fabs((double)relativeY) / halfHeight;

                if ((normalisedX + normalisedY) <= 1.0f)
                {
                    closestPoint = player.position;
                }
                else
                {
                    float scale = 1.0f / (normalisedX + normalisedY);
                    closestPoint.x = world.tiles[i].bounds.center.x + relativeX * scale;
                    closestPoint.y = world.tiles[i].bounds.center.y + relativeY * scale;
                }

                float tileDiffX = closestPoint.x - player.position.x;
                float tileDiffY = closestPoint.y - player.position.y;

                float isometricTileDiffY = tileDiffY * 2.0f;
                float tileDistanceSquared = tileDiffX * tileDiffX + isometricTileDiffY * isometricTileDiffY;

                float reach = PLAYER_REACH * WORLD_TILE_SIZE / 2;

                if (tileDistanceSquared < reach * reach)
                {
                    hoveredTile = world.tiles[i];
                    world.tiles[i].hovered = true;
                    tileFound = true;
                    hoveredIndex = i;
                }

                // only show the hover overlay for the "HAND" cursor
                if (strcmp(hoveredTile.cursorType, "HAND") == 0)
                {
                    Texture2D tileHoverTex = GetTexture(tileHoverSprite);
                    DrawTextureV(tileHoverTex,
                                 {world.tiles[i].bounds.center.x - WORLD_TILE_SIZE / 2,
                                  world.tiles[i].bounds.center.y - WORLD_TILE_SIZE / 4},
                                 WHITE);
                }

                goto tile_check_done;
            }
        }
    }

tile_check_done:

    float distance = 0.0f;

    if (showDebug)
    {
        // debug
        char mouseScreenPosBuffer[64];
        snprintf(mouseScreenPosBuffer, sizeof(mouseScreenPosBuffer), "Mouse Position (Screen): X:%.1f Y:%.1f", mouseScreenPos.x, mouseScreenPos.y);
        SetDebugValue("mouse_position_screen", mouseScreenPosBuffer);

        char mouseWorldPosBuffer[64];
        snprintf(mouseWorldPosBuffer, sizeof(mouseWorldPosBuffer), "Mouse Position (World): X:%.1f Y:%.1f", mouseWorldPos.x, mouseWorldPos.y);
        SetDebugValue("mouse_position_world", mouseWorldPosBuffer);

        distance = sqrtf(distanceSquared);
        char distanceTilesBuffer[64];
        float distanceInTiles = distance / WORLD_TILE_SIZE;
        snprintf(distanceTilesBuffer, sizeof(distanceTilesBuffer), "Mouse Distance (Tiles): %.2f", distanceInTiles);
        SetDebugValue("mouse_distance_tiles", distanceTilesBuffer);
    }

    if (tileFound)
    {
        char tileInfo[64];
        snprintf(tileInfo, sizeof(tileInfo), "Hovered Tile: %s (%s)", hoveredTile.name, hoveredTile.cursorType);
        SetDebugValue("hovered_tile", tileInfo);
        SetCurrentCursorSprite(hoveredTile.cursorType);

        player.hovering = hoveredTile;
        player.hoveringIndex = hoveredIndex;
    }
    else
    {
        SetDebugValue("hovered_tile", "Hovered Tile: None (POINTER)");
        SetCurrentCursorSprite("POINTER");

        player.hovering = {};
        player.hoveringIndex = -1;
    }
}

void CleanupWorld()
{
    World_Save(WORLD_FILE_NAME);

    CleanupTextureShader();

    if (world.tiles)
    {
        free(world.tiles);
        world.tiles = NULL;
    }
}

void PlaceTile(int tileIndex, unsigned char itemID)
{
    WorldTile *tile = &world.tiles[tileIndex];
    Item item = GetItemByID(itemID);
    TileType tileToPlace = GetTileByID(item.tileID);

    snprintf(tile->name, sizeof(tile->name), "%s", tileToPlace.name.c_str());
    strncpy(tile->sprite, tileToPlace.spritePath.c_str(), sizeof(tile->sprite) - 1);
    tile->sprite[sizeof(tile->sprite) - 1] = '\0';
    snprintf(tile->cursorType, sizeof(tile->cursorType), "%s", tileToPlace.cursorType);
    tile->useShader = tileToPlace.useShader;
    snprintf(tile->largeTexturePath, sizeof(tile->largeTexturePath), "%s", tileToPlace.largeTexturePath.c_str());

    Texture2D existingTex = GetTexture(tile->sprite);

    if (existingTex.id <= 0)
    {
        Image tileImage = LoadImage(tile->sprite);
        ImageResizeNN(&tileImage,
                      (int)((float)tileImage.width * tileToPlace.spriteScale),
                      (int)((float)tileImage.height * tileToPlace.spriteScale));

        RegisterTexture(tileImage, tile->sprite);
        UnloadImage(tileImage);
    }
}

void DestroyTile(int tileIndex)
{
    world.tiles[tileIndex] = emptyTile;
}