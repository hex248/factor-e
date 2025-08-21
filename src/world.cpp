#include "world.h"
#include "config.h"
#include "ui.h"
#include "mouse.h"
#include "player.h"
#include "controls.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "include/json.hpp"
#include <iostream>
#include <cmath>

Image noise;
Texture2D noiseTexture;

static Texture2D tileHoverSprite;
static LgTexShaderData lgTexShader;

using json = nlohmann::json;

// json parsing and serialization functions for each type
void to_json(json &j, const CollisionType &c)
{
    j = json{{"solid", c.solid}, {"width", c.width}, {"height", c.height}};
}

void from_json(const json &j, CollisionType &c)
{
    j.at("solid").get_to(c.solid);
    j.at("width").get_to(c.width);
    j.at("height").get_to(c.height);
}

void to_json(json &j, const TileType &t)
{
    j = json{{"name", t.name},
             {"description", t.description},
             {"layer", t.layer},
             {"spritePath", t.spritePath},
             {"spriteScale", t.spriteScale},
             {"collision", t.collision},
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
    j.at("collision").get_to(t.collision);
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

// get tile set data from json file
std::ifstream tile_sets_json(TILE_SETS_PATH);
json tile_sets_data = json::parse(tile_sets_json);

std::ifstream tile_types_json(TILE_TYPES_PATH);
json tile_types_data = json::parse(tile_types_json);

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
    Color pixelColor = GetImageColor(noiseImage, x * MAP_TILE_SIZE, y * MAP_TILE_SIZE);

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
                  (int)(image.width * 4),
                  (int)(image.height * 4));

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    lgTexShader.textures[texturePath] = texture;
    return texture;
}

void InitWorld(Map *map)
{
    int seedX = GetRandomValue(0, 10000000);
    int seedY = GetRandomValue(0, 10000000);

    // generate a tiny perlin noise image (using map size, so that it is 1 pixel per tile)
    noise = GenImagePerlinNoise(MAP_SIZE_X, MAP_SIZE_Y, seedX, seedY, 1.5);
    ImageResizeNN(&noise, MAP_SIZE_X * MAP_TILE_SIZE, MAP_SIZE_Y * MAP_TILE_SIZE);
    noiseTexture = LoadTextureFromImage(noise);

    InitTextureShader();

    Image tileHoverImage = LoadImage(TILE_HOVER_SPRITE_PATH);

    ImageResizeNN(&tileHoverImage,
                  (int)((float)tileHoverImage.width * TILE_HOVER_SPRITE_SCALE),
                  (int)((float)tileHoverImage.height * TILE_HOVER_SPRITE_SCALE));

    tileHoverSprite = LoadTextureFromImage(tileHoverImage);
    UnloadImage(tileHoverImage);

    TileSet tile_set = tile_sets_data["field_biome"].template get<TileSet>();

    map->tilesX = MAP_SIZE_X;
    map->tilesY = MAP_SIZE_Y;

    // TODO: implement multiple layers

    std::vector<std::string> layers = {"ground"};

    map->tiles = (WorldTile *)calloc(MAP_SIZE_X * MAP_SIZE_Y, sizeof(WorldTile));
    for (int y = 0; y < MAP_SIZE_Y; y++)
    {
        for (int x = 0; x < MAP_SIZE_X; x++)
        {
            int i = y * MAP_SIZE_X + x;

            TileType tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            // TODO: IMPLEMENT RULES
            // if the tile's layer is not in the layers vector, get a new tile
            while (std::find(layers.begin(), layers.end(), tile.layer) == layers.end())
            {
                tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            }
            snprintf(map->tiles[i].name, sizeof(map->tiles[i].name), "%s", tile.name.c_str());
            unsigned char lightness = (unsigned char)GetRandomValue(0, 120);
            map->tiles[i].color = {lightness, (unsigned char)(lightness + 80), lightness, 255};

            if (!tile.useShader)
            {
                Image spriteImage = LoadImage(tile.spritePath.c_str());

                ImageResizeNN(&spriteImage,
                              (int)((float)spriteImage.width * tile.spriteScale),
                              (int)((float)spriteImage.height * tile.spriteScale));

                map->tiles[i].sprite = LoadTextureFromImage(spriteImage);
                UnloadImage(spriteImage);
            }
            snprintf(map->tiles[i].cursorType, sizeof(map->tiles[i].cursorType), "%s", tile.cursorType);
            map->tiles[i].useShader = tile.useShader;
            snprintf(map->tiles[i].largeTexturePath, sizeof(map->tiles[i].largeTexturePath), "%s", tile.largeTexturePath.c_str());
        }
    }
}

void DrawWorld(Map *map)
{
    // position grid at a fixed world coordinate (centered around 0,0)
    float offsetX = -(MAP_TILE_SIZE * MAP_SIZE_X) / 2.0f;
    float offsetY = -(MAP_TILE_SIZE * MAP_SIZE_Y) / 2.0f;

    // draw tiles
    for (int y = 0; y < map->tilesY; y++)
    {
        for (int x = 0; x < map->tilesX; x++)
        {
            int tileX = (int)(offsetX + (float)x * MAP_TILE_SIZE);
            int tileY = (int)(offsetY + (float)y * MAP_TILE_SIZE);
            int index = y * map->tilesX + x;

            DrawRectangle(tileX, tileY, MAP_TILE_SIZE, MAP_TILE_SIZE, map->tiles[index].color);

            // if the tile should use a shader and has a largeTexturePath
            if (map->tiles[index].useShader && strlen(map->tiles[index].largeTexturePath) > 0 && lgTexShader.initialized)
            {
                Texture2D lgTex = GetOrLoadShaderTexture(std::string(map->tiles[index].largeTexturePath));

                // if the texture successfully loads
                if (lgTex.id > 0)
                {
                    BeginShaderMode(lgTexShader.shader);

                    // set shader values
                    float tileSize = (float)MAP_TILE_SIZE;
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
                        {(float)tileX, (float)tileY, MAP_TILE_SIZE, MAP_TILE_SIZE},
                        {0, 0},
                        0.0f,
                        WHITE);

                    EndShaderMode();
                }
                else
                {
                    // fallback to regular sprite if shader texture failed to load
                    Texture2D sprite = map->tiles[index].sprite;
                    DrawTexture(sprite, tileX - (sprite.width / 2) + MAP_TILE_SIZE / 2, tileY - (sprite.height / 2) + MAP_TILE_SIZE / 2, WHITE);
                }
            }
            // no shader
            else
            {
                Texture2D sprite = map->tiles[index].sprite;
                DrawTexture(sprite, tileX - (sprite.width / 2) + MAP_TILE_SIZE / 2, tileY - (sprite.height / 2) + MAP_TILE_SIZE / 2, WHITE);
            }

            map->tiles[index].bounds = {(float)tileX, (float)tileY, MAP_TILE_SIZE, MAP_TILE_SIZE};
        }
    }

    // draw grid lines
    if (showDebug)
    {
        int gridStartX = (int)offsetX;
        int gridStartY = (int)offsetY;
        int gridEndX = gridStartX + MAP_SIZE_X * MAP_TILE_SIZE;
        int gridEndY = gridStartY + MAP_SIZE_Y * MAP_TILE_SIZE;

        for (int x = 0; x <= MAP_SIZE_X; x++)
        {
            int lineX = gridStartX + x * MAP_TILE_SIZE;
            DrawLine(lineX, gridStartY, lineX, gridEndY, {255, 255, 255, 60});
        }

        for (int y = 0; y <= MAP_SIZE_Y; y++)
        {
            int lineY = gridStartY + y * MAP_TILE_SIZE;
            DrawLine(gridStartX, lineY, gridEndX, lineY, {255, 255, 255, 60});
        }

        // draw noise map
        // DrawTexture(noiseTexture, 0 - noiseTexture.width / 2, 0 - noiseTexture.height / 2, WHITE);
    }
}

void CheckHover(Map *map)
{
    Vector2 mouseWorldPos = GetMouseWorldPosition();
    Vector2 mouseScreenPos = GetMousePosition();
    WorldTile hoveredTile;
    bool tileFound = false;

    float diffX = mouseWorldPos.x - player.position.x;
    float diffY = mouseWorldPos.y - player.position.y;
    float distanceSquared = diffX * diffX + diffY * diffY;

    for (int i = 0; i < map->tilesX * map->tilesY; i++)
    {
        if (CheckCollisionPointRec(mouseWorldPos, map->tiles[i].bounds))
        {
            // find the closest point in the tile bounds to the player
            Vector2 closestPoint;
            closestPoint.x = fmaxf(map->tiles[i].bounds.x,
                                   fminf(player.position.x,
                                         map->tiles[i].bounds.x + map->tiles[i].bounds.width));
            closestPoint.y = fmaxf(map->tiles[i].bounds.y,
                                   fminf(player.position.y,
                                         map->tiles[i].bounds.y + map->tiles[i].bounds.height));

            float tileDiffX = closestPoint.x - player.position.x;
            float tileDiffY = closestPoint.y - player.position.y;
            float tileDistanceSquared = tileDiffX * tileDiffX + tileDiffY * tileDiffY;

            float reach = PLAYER_REACH * MAP_TILE_SIZE;

            if (tileDistanceSquared < reach * reach)
            {
                hoveredTile = map->tiles[i];
                map->tiles[i].hovered = true;
                tileFound = true;
            }

            if (strcmp(hoveredTile.cursorType, "HAND") == 0) // only show the hover overlay for the "HAND" cursor
                DrawTextureV(tileHoverSprite, {map->tiles[i].bounds.x, map->tiles[i].bounds.y}, WHITE);
            break;
        }
        map->tiles[i].hovered = false;
    }

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
        float distanceInTiles = distance / MAP_TILE_SIZE;
        snprintf(distanceTilesBuffer, sizeof(distanceTilesBuffer), "Mouse Distance (Tiles): %.2f", distanceInTiles);
        SetDebugValue("mouse_distance_tiles", distanceTilesBuffer);
    }

    if (tileFound)
    {
        char tileInfo[64];
        snprintf(tileInfo, sizeof(tileInfo), "Hovered Tile: %s (%s)", hoveredTile.name, hoveredTile.cursorType);
        SetDebugValue("hovered_tile", tileInfo);
        SetCurrentCursorSprite(hoveredTile.cursorType);
    }
    else
    {
        SetDebugValue("hovered_tile", "Hovered Tile: None (POINTER)");
        SetCurrentCursorSprite("POINTER");
    }

    // draw debug line between player and mouse position
    if (showDebug)
    {
        // blue line: player's maximum reach in the direction of the mouse
        float reach = PLAYER_REACH * MAP_TILE_SIZE;
        float reachSquared = reach * reach;
        Vector2 direction = {diffX, diffY};

        Vector2 reachEndPoint;
        // only calculate reachEndPoint if mouse is outside player's reach (sqrt scary)
        if (distanceSquared > reachSquared)
        {
            // only draw red line if mouse is out of player reach
            // red line: full distance to mouse
            DrawLineEx(player.position, mouseWorldPos, 2.0f, RED);

            // normalize direction and scale to reach distance
            float scaleFactor = reach / distance;
            reachEndPoint = {
                player.position.x + direction.x * scaleFactor,
                player.position.y + direction.y * scaleFactor};
        }
        else
            reachEndPoint = mouseWorldPos;

        DrawLineEx(player.position, reachEndPoint, 2.0f, BLUE);
    }
}

void CleanupWorld(Map *map)
{
    CleanupTextureShader();

    if (map->tiles)
    {
        free(map->tiles);
        map->tiles = NULL;
    }
}
