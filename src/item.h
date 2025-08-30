#include <string>
#include "include/json.hpp"
#include "raylib.h"

#define ITEMS_PATH "assets/data/items.json"
#define ITEM_STACK_WIDTH 160
#define ITEM_STACK_HEIGHT 160

typedef struct Item
{
    unsigned char id;
    std::string name;
    std::string iconSpritePath;
    std::string inHandSpritePath;
    float iconScale;
    float inHandScale;
    unsigned char tileID;
    unsigned int stackSize;
    bool placeable;
    bool showInHand;
} Item;

typedef struct ItemStack
{
    unsigned char itemID;
    unsigned int quantity;
    Texture2D iconSprite;
    Texture2D inHandSprite;
} ItemStack;

void to_json(nlohmann::json &j, const Item &item);
void from_json(const nlohmann::json &j, Item &item);

Item GetItemByID(unsigned char id);
Item GetItemByKey(const std::string &key);

ItemStack CreateItemStack(unsigned char itemID, unsigned int quantity);