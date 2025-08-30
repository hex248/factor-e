#include <fstream>
#include "item.h"

using json = nlohmann::json;

void to_json(json &j, const Item &item)
{
    j = json{
        {"id", item.id},
        {"name", item.name},
        {"iconSpritePath", item.iconSpritePath},
        {"inHandSpritePath", item.inHandSpritePath},
        {"iconScale", item.iconScale},
        {"inHandScale", item.inHandScale},
        {"tileID", item.tileID},
        {"stackSize", item.stackSize},
        {"placeable", item.placeable},
        {"showInHand", item.showInHand}};
}

void from_json(const json &j, Item &item)
{
    j.at("id").get_to(item.id);
    j.at("name").get_to(item.name);
    j.at("iconSpritePath").get_to(item.iconSpritePath);
    j.at("inHandSpritePath").get_to(item.inHandSpritePath);
    j.at("iconScale").get_to(item.iconScale);
    j.at("inHandScale").get_to(item.inHandScale);
    j.at("tileID").get_to(item.tileID);
    j.at("stackSize").get_to(item.stackSize);
    j.at("placeable").get_to(item.placeable);
    j.at("showInHand").get_to(item.showInHand);
}

// get items data from json file
std::ifstream items_json(ITEMS_PATH);
json items_data = json::parse(items_json);

Item GetItemByID(unsigned char id)
{
    for (const auto &item_json : items_data)
    {
        Item item;
        from_json(item_json, item);
        if (item.id == id)
        {
            return item;
        }
    }
    printf("Item with ID %d not found\n", id);
    // return empty item if not found
    return {0};
}

Item GetItemByKey(const std::string &key)
{
    return items_data[key].template get<Item>();
}

ItemStack CreateItemStack(unsigned char itemID, unsigned int quantity)
{
    Item item = GetItemByID(itemID);
    if (item.id == 0)
    {
        printf("Cannot create ItemStack: Item with ID %d not found\n", itemID);
        return {0, 0};
    }
    if (quantity > item.stackSize)
    {
        quantity = item.stackSize;
    }
    ItemStack stack;
    stack.itemID = itemID;
    stack.quantity = quantity;
    Image iconSpriteImage = LoadImage(item.iconSpritePath.c_str());
    if (iconSpriteImage.data != NULL)
    {
        ImageResizeNN(&iconSpriteImage,
                      ITEM_STACK_WIDTH,
                      ITEM_STACK_HEIGHT);

        stack.iconSprite = LoadTextureFromImage(iconSpriteImage);
        UnloadImage(iconSpriteImage);
    }
    Image inHandSpriteImage = LoadImage(item.inHandSpritePath.c_str());
    if (inHandSpriteImage.data != NULL)
    {
        ImageResizeNN(&inHandSpriteImage,
                      (int)((float)inHandSpriteImage.width * item.inHandScale),
                      (int)((float)inHandSpriteImage.height * item.inHandScale));

        stack.inHandSprite = LoadTextureFromImage(inHandSpriteImage);
        UnloadImage(inHandSpriteImage);
    }

    return stack;
}
