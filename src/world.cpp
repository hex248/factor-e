#include "world.h"
#include "config.h"
#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "include/json.hpp"
#include <iostream>

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
             {"collision", t.collision}};
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
}

void to_json(json &j, const TileSet &t)
{
    j = json{{"id", t.id},
             {"name", t.name},
             {"description", t.description},
             {"tiles", t.tiles},
             {"weights", t.weights},
             {"rules", t.rules}};
}

void from_json(const json &j, TileSet &t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("description").get_to(t.description);

    // loop through tiles array
    for (const auto &tile : j.at("tiles"))
    {
        // add to t.tiles string vector
        t.tiles.push_back(tile.get<std::string>());
    }
    for (const auto &tile : t.tiles)
    {
        std::cout << tile << " ";
    }
    std::cout << std::endl;

    for (const auto &item : j.at("weights").items())
    {
        t.weights[item.key()] = item.value().get<int>();
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
    for (const auto &rule : j.at("rules").items())
    {
        std::string tileName = rule.key();

        for (const auto &ruleType : rule.value().items())
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

TileType GetRandomWeightedTile(const std::map<std::string, int> &weights)
{
    int totalWeight = 0;
    for (const auto &[key, value] : weights)
    {
        totalWeight += value;
    }

    int randomValue = GetRandomValue(0, totalWeight - 1);

    // ?sort weights descending
    // this ensures that all weights are accounted for, even if the second weight is greater than the first
    // EXAMPLE:
    //     grass: 20
    //     dirt: 40
    // dirt has 40/60 chance and grass has 20/60
    // without sorting, grass would have been chosen much more frequently as it comes first.
    std::vector<std::pair<std::string, int>> sortedWeights;
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

void InitWorld(Map *map)
{
    TileSet tile_set = tile_sets_data["field_biome"].template get<TileSet>();

    map->tilesX = MAP_SIZE_X;
    map->tilesY = MAP_SIZE_Y;
    int toGenerate = MAP_SIZE_X * MAP_SIZE_Y;

    map->tileIds = (unsigned char *)calloc(toGenerate, sizeof(unsigned char));
    for (unsigned int i = 0; i < toGenerate; i++)
        map->tileIds[i] = i;

    /* //TODO: implement multiple layers
        requires multiple layers of "map->tiles" (2d array)
        multiple passes of the toGenerate loop
    */
    std::vector<std::string> layers = {"ground"};

    map->tiles = (WorldTile *)calloc(toGenerate, sizeof(WorldTile));
    for (unsigned int i = 0; i < toGenerate; i++)
    {
        map->tiles[i].id = i;

        TileType tile = GetRandomWeightedTile(tile_set.weights);
        // TODO: IMPLEMENT RULES
        // if the tile's layer is not in the layers vector, get a new tile
        while (std::find(layers.begin(), layers.end(), tile.layer) == layers.end())
        {
            tile = GetRandomWeightedTile(tile_set.weights);
        }
        snprintf(map->tiles[i].name, sizeof(map->tiles[i].name), "%s", tile.name.c_str());
        unsigned char lightness = (unsigned char)GetRandomValue(0, 120);
        map->tiles[i].color = (Color){lightness, (unsigned char)(lightness + 80), lightness, 255};
        Image spriteImage = LoadImage(tile.spritePath.c_str());

        ImageResizeNN(&spriteImage,
                      (int)(spriteImage.width * tile.spriteScale),
                      (int)(spriteImage.height * tile.spriteScale));

        map->tiles[i].sprite = LoadTextureFromImage(spriteImage);
        UnloadImage(spriteImage);
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
            int index = y * map->tilesX + x;

            DrawRectangle(tileX, tileY, MAP_TILE_SIZE, MAP_TILE_SIZE, map->tiles[index].color);

            Vector2 textSize = MeasureTextEx(fontSmall, map->tiles[index].name, (float)fontSmall.baseSize, 2);
            Vector2 textPos = {
                tileX + (MAP_TILE_SIZE - textSize.x) / 2,
                tileY + (MAP_TILE_SIZE - textSize.y) / 2};
            DrawTextEx(fontSmall, map->tiles[index].name, textPos, (float)fontSmall.baseSize, 2, WHITE);
            Texture2D sprite = map->tiles[index].sprite;
            DrawTexture(sprite, tileX - (sprite.width / 2) + MAP_TILE_SIZE / 2, tileY - (sprite.height / 2) + MAP_TILE_SIZE / 2, WHITE);
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
