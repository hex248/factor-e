#include <string>
#include "include/json.hpp"
#include "raylib.h"
#include <string>

#define ITEMS_PATH "assets/data/items.json"
#define ITEM_STACK_WIDTH 160.0f
#define ITEM_STACK_HEIGHT 160.0f

#define TOOLS_PATH "assets/data/tools.json"

typedef struct Item
{
    unsigned char id;
    std::string name;
    std::string iconSpritePath;
    std::string inHandSpritePath;
    Vector2 inHandOffset;
    float iconScale;
    float inHandScale;
    unsigned char tileID;
    unsigned int stackSize;
    bool isPlaceable;
    bool showInHand;
    bool isTool;
} Item;

typedef struct ItemStack
{
    unsigned char itemID;
    unsigned int quantity;
    std::string iconSprite;
    std::string inHandSprite;
    Vector2 inHandOffset;
    bool showInHand;
    bool isTool;
} ItemStack;

typedef struct Tool
{
    unsigned char id;
    unsigned char itemID;
    std::string name;
    std::vector<std::string> targets;
    std::string function;
    float speed;
} Tool;

void to_json(nlohmann::json &j, const Item &item);
void from_json(const nlohmann::json &j, Item &item);

Item GetItemByID(unsigned char id);
Item GetItemByKey(const std::string &key);

ItemStack CreateItemStack(unsigned char itemID, unsigned int quantity);

void to_json(nlohmann::json &j, const Tool &tool);
void from_json(const nlohmann::json &j, Tool &tool);

Tool GetToolByName(const std::string &name);