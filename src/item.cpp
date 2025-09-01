#include <fstream>
#include "item.h"
#include "tex.h"

using json = nlohmann::json;

void to_json(json &j, const Item &item)
{
    j = json{
        {"id", item.id},
        {"name", item.name},
        {"iconSpritePath", item.iconSpritePath},
        {"inHandSpritePath", item.inHandSpritePath},
        {"inHandOffset", {{"x", item.inHandOffset.x}, {"y", item.inHandOffset.y}}},
        {"iconScale", item.iconScale},
        {"inHandScale", item.inHandScale},
        {"tileID", item.tileID},
        {"stackSize", item.stackSize},
        {"isPlaceable", item.isPlaceable},
        {"showInHand", item.showInHand},
        {"isTool", item.isTool}};
}

void from_json(const json &j, Item &item)
{
    j.at("id").get_to(item.id);
    j.at("name").get_to(item.name);
    j.at("iconSpritePath").get_to(item.iconSpritePath);
    j.at("inHandSpritePath").get_to(item.inHandSpritePath);
    j.at("inHandOffset").at("x").get_to(item.inHandOffset.x);
    j.at("inHandOffset").at("y").get_to(item.inHandOffset.y);
    j.at("iconScale").get_to(item.iconScale);
    j.at("inHandScale").get_to(item.inHandScale);
    j.at("tileID").get_to(item.tileID);
    j.at("stackSize").get_to(item.stackSize);
    j.at("isPlaceable").get_to(item.isPlaceable);
    j.at("showInHand").get_to(item.showInHand);
    j.at("isTool").get_to(item.isTool);
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
    return {};
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
        return {};
    }
    if (quantity > item.stackSize)
    {
        quantity = item.stackSize;
    }
    ItemStack stack;
    stack.itemID = itemID;
    stack.quantity = quantity;

    Texture2D iconTex = GetTexture(item.iconSpritePath);
    if (iconTex.id <= 0)
    {
        Image iconSpriteImage = LoadImage(item.iconSpritePath.c_str());
        if (iconSpriteImage.data != NULL)
        {
            ImageResizeNN(&iconSpriteImage,
                          ITEM_STACK_WIDTH,
                          ITEM_STACK_HEIGHT);

            stack.iconSprite = RegisterTexture(iconSpriteImage, item.iconSpritePath);
            UnloadImage(iconSpriteImage);
        }
    }

    stack.showInHand = item.showInHand;
    if (item.showInHand)
    {
        stack.inHandSprite = item.inHandSpritePath;
        Texture2D inHandTex = GetTexture(item.inHandSpritePath);
        if (inHandTex.id <= 0)
        {
            Image inHandSpriteImage = LoadImage(item.inHandSpritePath.c_str());
            if (inHandSpriteImage.data != NULL)
            {
                ImageResizeNN(&inHandSpriteImage,
                              (int)((float)inHandSpriteImage.width * item.inHandScale),
                              (int)((float)inHandSpriteImage.height * item.inHandScale));

                RegisterTexture(inHandSpriteImage, item.inHandSpritePath);
                UnloadImage(inHandSpriteImage);
            }
        }

        stack.inHandOffset = item.inHandOffset;
    }

    stack.isPlaceable = item.isPlaceable;
    stack.isTool = item.isTool;

    return stack;
}

void to_json(nlohmann::json &j, const Tool &tool)
{
    j = json{{"id", tool.id},
             {"itemID", tool.itemID},
             {"name", tool.name},
             {"targets", tool.targets},
             {"function", tool.function},
             {"speed", tool.speed}};
}

void from_json(const nlohmann::json &j, Tool &tool)
{
    j.at("id").get_to(tool.id);
    j.at("itemID").get_to(tool.itemID);
    j.at("name").get_to(tool.name);
    j.at("targets").get_to(tool.targets);
    j.at("function").get_to(tool.function);
    j.at("speed").get_to(tool.speed);
}

// get tools data from json file
std::ifstream tools_json(TOOLS_PATH);
json tools_data = json::parse(tools_json);

Tool GetToolByName(const std::string &name)
{
    std::string lowercaseName = name;
    std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);

    return tools_data[lowercaseName].template get<Tool>();
}
