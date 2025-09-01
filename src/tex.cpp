#include "tex.h"
#include <stdio.h>
#include <map>
#include <string>

std::map<unsigned char, Texture2D> loadedTextures;
std::map<std::string, Texture2D> customKeyTextures;

unsigned char RegisterTexture(Image img)
{
    unsigned char newKey = (unsigned char)(loadedTextures.size()) + 1;
    loadedTextures[newKey] = LoadTextureFromImage(img);

    return newKey;
}

unsigned char RegisterTexture(const char *file)
{
    unsigned char newKey = (unsigned char)(loadedTextures.size()) + 1;
    loadedTextures[newKey] = LoadTexture(file);

    return newKey;
}

std::string RegisterTexture(Image img, std::string customKey)
{
    customKeyTextures[customKey] = LoadTextureFromImage(img);

    return customKey;
}

std::string RegisterTexture(const char *file, std::string customKey)
{
    customKeyTextures[customKey] = LoadTexture(file);

    return customKey;
}

Texture2D GetTexture(unsigned char key)
{
    return loadedTextures[key];
}

Texture2D GetTexture(std::string customKey)
{
    return customKeyTextures[customKey];
}

void UnregisterTextures()
{
    for (auto texture : loadedTextures)
    {
        UnloadTexture(texture.second);
        loadedTextures.erase(texture.first);
    }
    for (auto texture : customKeyTextures)
    {
        UnloadTexture(texture.second);
        customKeyTextures.erase(texture.first);
    }
}

void UnregisterTexture(unsigned char key)
{
    loadedTextures.erase(key);
}

void UnregisterTexture(std::string customKey)
{
    customKeyTextures.erase(customKey);
}