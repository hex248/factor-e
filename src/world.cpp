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

// get tile set data from json file
std::ifstream tile_sets_json(TILE_SETS_PATH);
json tile_sets_data = json::parse(tile_sets_json);

std::ifstream tile_types_json(TILE_TYPES_PATH);
json tile_types_data = json::parse(tile_types_json);

void InitWorld(Map *map)
{
    for (auto &[key, value] : tile_types_data.items())
    {
        auto item = value.template get<TileType>();
        printf("name: %s, description: %s\n", item.name.c_str(), item.description.c_str());
    }
    auto rock = tile_types_data["rock"].template get<TileType>();

    map->tilesX = MAP_SIZE_X;
    map->tilesY = MAP_SIZE_Y;

    map->tileIds = (unsigned char *)calloc(map->tilesX * map->tilesY, sizeof(unsigned char));
    for (unsigned int i = 0; i < map->tilesY * map->tilesX; i++)
        map->tileIds[i] = i;

    map->tiles = (WorldTile *)calloc(map->tilesX * map->tilesY, sizeof(WorldTile));
    for (unsigned int i = 0; i < map->tilesY * map->tilesX; i++)
    {
        map->tiles[i].id = i;
        // snprintf(map->tiles[i].name, sizeof(map->tiles[i].name), "%d", i + 1);
        snprintf(map->tiles[i].name, sizeof(map->tiles[i].name), "%s", rock.name.c_str());
        int lightness = GetRandomValue(0, 120);
        map->tiles[i].color = (Color){lightness, lightness + 80, lightness, 255};
        Image spriteImage = LoadImage(rock.spritePath.c_str());

        ImageResizeNN(&spriteImage,
                      (int)(spriteImage.width * rock.spriteScale),
                      (int)(spriteImage.height * rock.spriteScale));

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
