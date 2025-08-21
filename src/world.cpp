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
#include <algorithm>

#include <stdint.h>
#include <memory.h>
#include <string.h>

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

void RegenerateTexturesForLoadedWorld(World *world)
{
    for (int i = 0; i < world->tilesX * world->tilesY; i++)
    {
        if (!world->tiles[i].useShader && strlen(world->tiles[i].name) > 0)
        {
            std::string tileName = world->tiles[i].name;

            // convert to lowercase
            std::transform(tileName.begin(), tileName.end(), tileName.begin(), ::tolower);

            bool tileTypeFound = false;
            for (auto &[key, value] : tile_types_data.items())
            {
                if (key == tileName)
                {
                    TileType tileType = value.template get<TileType>();

                    Image spriteImage = LoadImage(tileType.spritePath.c_str());
                    if (spriteImage.data != NULL)
                    {
                        ImageResizeNN(&spriteImage,
                                      (int)((float)spriteImage.width * tileType.spriteScale),
                                      (int)((float)spriteImage.height * tileType.spriteScale));

                        world->tiles[i].sprite = LoadTextureFromImage(spriteImage);
                        UnloadImage(spriteImage);
                    }

                    tileTypeFound = true;
                    break;
                }
            }

            if (!tileTypeFound)
            {
                printf("WARNING: could not find tile type for '%s' during texture regeneration\n", world->tiles[i].name);
                // invalid texture as fallback
                world->tiles[i].sprite.id = 0;
                world->tiles[i].sprite.width = 0;
                world->tiles[i].sprite.height = 0;
                world->tiles[i].sprite.mipmaps = 0;
                world->tiles[i].sprite.format = 0;
            }
        }
    }
}

int World_Save(const char *path, struct World *world)
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
    written = fwrite(&world->tilesX, sizeof(world->tilesX), 1, fp);
    if (written != 1)
    {
        printf("WORLD SAVE: failed to write tilesX to \"%s\"", path);
        fclose(fp);
        return 0;
    }

    written = fwrite(&world->tilesY, sizeof(world->tilesY), 1, fp);
    if (written != 1)
    {
        printf("WORLD SAVE: failed to write tilesY to \"%s\"", path);
        fclose(fp);
        return 0;
    }

    // save the tiles array data
    if (world->tiles != NULL)
    {
        size_t tilesCount = world->tilesX * world->tilesY;
        written = fwrite(world->tiles, sizeof(WorldTile), tilesCount, fp);
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

int World_Load(const char *path, World *world)
{
    FILE *fp;
    size_t num_read;

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("WORLD LOAD: failed to fopen file \"%s\"", path);
        return 0;
    }

    num_read = fread(&world->tilesX, sizeof(world->tilesX), 1, fp);
    if (num_read != 1)
    {
        printf("WORLD LOAD: failed to read tilesX from \"%s\"", path);
        fclose(fp);
        return 0;
    }

    num_read = fread(&world->tilesY, sizeof(world->tilesY), 1, fp);
    if (num_read != 1)
    {
        printf("WORLD LOAD: failed to read tilesY from \"%s\"", path);
        fclose(fp);
        return 0;
    }

    size_t tilesCount = world->tilesX * world->tilesY;
    world->tiles = (WorldTile *)calloc(tilesCount, sizeof(WorldTile));
    if (world->tiles == NULL)
    {
        printf("WORLD LOAD: failed to allocate memory for tiles");
        fclose(fp);
        return 0;
    }

    num_read = fread(world->tiles, sizeof(WorldTile), tilesCount, fp);
    if (num_read != tilesCount)
    {
        printf("WORLD LOAD: failed to read tiles data from \"%s\"", path);
        free(world->tiles);
        world->tiles = NULL;
        fclose(fp);
        return 0;
    }

    fclose(fp);

    InitTextureShader();

    RegenerateTexturesForLoadedWorld(world);

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
                  (int)(image.width * 4),
                  (int)(image.height * 4));

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    lgTexShader.textures[texturePath] = texture;
    return texture;
}

void GenerateWorld(World *world)
{
    // generate a tiny perlin noise image (using world size, so that it is 1 pixel per tile)
    int seedX = GetRandomValue(0, 10000000);
    int seedY = GetRandomValue(0, 10000000);
    noise = GenImagePerlinNoise(WORLD_SIZE_X, WORLD_SIZE_Y, seedX, seedY, 1.5);
    ImageResizeNN(&noise, WORLD_SIZE_X * WORLD_TILE_SIZE, WORLD_SIZE_Y * WORLD_TILE_SIZE);
    noiseTexture = LoadTextureFromImage(noise);

    InitTextureShader();

    TileSet tile_set = tile_sets_data["field_biome"].template get<TileSet>();

    world->tilesX = WORLD_SIZE_X;
    world->tilesY = WORLD_SIZE_Y;

    // TODO: implement multiple layers

    std::vector<std::string> layers = {"ground"};

    world->tiles = (WorldTile *)calloc(WORLD_SIZE_X * WORLD_SIZE_Y, sizeof(WorldTile));
    for (int y = 0; y < WORLD_SIZE_Y; y++)
    {
        for (int x = 0; x < WORLD_SIZE_X; x++)
        {
            int i = y * WORLD_SIZE_X + x;

            TileType tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            // TODO: IMPLEMENT RULES
            // if the tile's layer is not in the layers vector, get a new tile
            while (std::find(layers.begin(), layers.end(), tile.layer) == layers.end())
            {
                tile = GetTileFromNoiseWeighted(x, y, noise, tile_set.weights);
            }
            snprintf(world->tiles[i].name, sizeof(world->tiles[i].name), "%s", tile.name.c_str());
            unsigned char lightness = (unsigned char)GetRandomValue(0, 120);
            world->tiles[i].color = {lightness, (unsigned char)(lightness + 80), lightness, 255};

            if (!tile.useShader)
            {
                Image spriteImage = LoadImage(tile.spritePath.c_str());

                ImageResizeNN(&spriteImage,
                              (int)((float)spriteImage.width * tile.spriteScale),
                              (int)((float)spriteImage.height * tile.spriteScale));

                world->tiles[i].sprite = LoadTextureFromImage(spriteImage);
                UnloadImage(spriteImage);
            }
            snprintf(world->tiles[i].cursorType, sizeof(world->tiles[i].cursorType), "%s", tile.cursorType);
            world->tiles[i].useShader = tile.useShader;
            snprintf(world->tiles[i].largeTexturePath, sizeof(world->tiles[i].largeTexturePath), "%s", tile.largeTexturePath.c_str());
        }
    }

    World_Save(WORLD_FILE_NAME, world);
}

void InitWorld(World *world)
{
    Image tileHoverImage = LoadImage(TILE_HOVER_SPRITE_PATH);

    ImageResizeNN(&tileHoverImage,
                  (int)((float)tileHoverImage.width * TILE_HOVER_SPRITE_SCALE),
                  (int)((float)tileHoverImage.height * TILE_HOVER_SPRITE_SCALE));

    tileHoverSprite = LoadTextureFromImage(tileHoverImage);
    UnloadImage(tileHoverImage);

    if (FileExists(WORLD_FILE_NAME))
    {
        if (World_Load(WORLD_FILE_NAME, world) != 1)
        {
            printf("Failed to load world, generating new one\n");
            GenerateWorld(world);
        }
    }
    else
    {
        GenerateWorld(world);
    }
}

void DrawWorld(World *world)
{
    // position grid at a fixed world coordinate (centered around 0,0)
    float offsetX = -(WORLD_TILE_SIZE * WORLD_SIZE_X) / 2.0f;
    float offsetY = -(WORLD_TILE_SIZE * WORLD_SIZE_Y) / 2.0f;

    // draw tiles
    for (int y = 0; y < world->tilesY; y++)
    {
        for (int x = 0; x < world->tilesX; x++)
        {
            int tileX = (int)(offsetX + (float)x * WORLD_TILE_SIZE);
            int tileY = (int)(offsetY + (float)y * WORLD_TILE_SIZE);
            int index = y * world->tilesX + x;

            DrawRectangle(tileX, tileY, WORLD_TILE_SIZE, WORLD_TILE_SIZE, world->tiles[index].color);

            // if the tile should use a shader and has a largeTexturePath
            if (world->tiles[index].useShader && strlen(world->tiles[index].largeTexturePath) > 0 && lgTexShader.initialized)
            {
                Texture2D lgTex = GetOrLoadShaderTexture(std::string(world->tiles[index].largeTexturePath));

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
                        {(float)tileX, (float)tileY, WORLD_TILE_SIZE, WORLD_TILE_SIZE},
                        {0, 0},
                        0.0f,
                        WHITE);

                    EndShaderMode();
                }
                else
                {
                    // fallback to regular sprite if shader texture failed to load
                    Texture2D sprite = world->tiles[index].sprite;
                    DrawTexture(sprite, tileX - (sprite.width / 2) + WORLD_TILE_SIZE / 2, tileY - (sprite.height / 2) + WORLD_TILE_SIZE / 2, WHITE);
                }
            }
            // no shader
            else
            {
                Texture2D sprite = world->tiles[index].sprite;
                DrawTexture(sprite, tileX - (sprite.width / 2) + WORLD_TILE_SIZE / 2, tileY - (sprite.height / 2) + WORLD_TILE_SIZE / 2, WHITE);
            }

            world->tiles[index].bounds = {(float)tileX, (float)tileY, WORLD_TILE_SIZE, WORLD_TILE_SIZE};
        }
    }

    // draw grid lines
    if (showDebug)
    {
        int gridStartX = (int)offsetX;
        int gridStartY = (int)offsetY;
        int gridEndX = gridStartX + WORLD_SIZE_X * WORLD_TILE_SIZE;
        int gridEndY = gridStartY + WORLD_SIZE_Y * WORLD_TILE_SIZE;

        for (int x = 0; x <= WORLD_SIZE_X; x++)
        {
            int lineX = gridStartX + x * WORLD_TILE_SIZE;
            DrawLine(lineX, gridStartY, lineX, gridEndY, {255, 255, 255, 60});
        }

        for (int y = 0; y <= WORLD_SIZE_Y; y++)
        {
            int lineY = gridStartY + y * WORLD_TILE_SIZE;
            DrawLine(gridStartX, lineY, gridEndX, lineY, {255, 255, 255, 60});
        }
    }
}

void CheckHover(World *world)
{
    Vector2 mouseWorldPos = GetMouseWorldPosition();
    Vector2 mouseScreenPos = GetMousePosition();
    WorldTile hoveredTile;
    bool tileFound = false;

    float diffX = mouseWorldPos.x - player.position.x;
    float diffY = mouseWorldPos.y - player.position.y;
    float distanceSquared = diffX * diffX + diffY * diffY;

    for (int i = 0; i < world->tilesX * world->tilesY; i++)
    {
        if (CheckCollisionPointRec(mouseWorldPos, world->tiles[i].bounds))
        {
            // find the closest point in the tile bounds to the player
            Vector2 closestPoint;
            closestPoint.x = fmaxf(world->tiles[i].bounds.x,
                                   fminf(player.position.x,
                                         world->tiles[i].bounds.x + world->tiles[i].bounds.width));
            closestPoint.y = fmaxf(world->tiles[i].bounds.y,
                                   fminf(player.position.y,
                                         world->tiles[i].bounds.y + world->tiles[i].bounds.height));

            float tileDiffX = closestPoint.x - player.position.x;
            float tileDiffY = closestPoint.y - player.position.y;
            float tileDistanceSquared = tileDiffX * tileDiffX + tileDiffY * tileDiffY;

            float reach = PLAYER_REACH * WORLD_TILE_SIZE;

            if (tileDistanceSquared < reach * reach)
            {
                hoveredTile = world->tiles[i];
                world->tiles[i].hovered = true;
                tileFound = true;
            }

            if (strcmp(hoveredTile.cursorType, "HAND") == 0) // only show the hover overlay for the "HAND" cursor
                DrawTextureV(tileHoverSprite, {world->tiles[i].bounds.x, world->tiles[i].bounds.y}, WHITE);
            break;
        }
        world->tiles[i].hovered = false;
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
        float reach = PLAYER_REACH * WORLD_TILE_SIZE;
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

void CleanupWorld(World *world)
{
    World_Save(WORLD_FILE_NAME, world);

    CleanupTextureShader();

    if (world->tiles)
    {
        // clean up individual tile textures before freeing the array
        for (int i = 0; i < world->tilesX * world->tilesY; i++)
        {
            if (world->tiles[i].sprite.id > 0)
            {
                UnloadTexture(world->tiles[i].sprite);
            }
        }

        free(world->tiles);
        world->tiles = NULL;
    }
}
