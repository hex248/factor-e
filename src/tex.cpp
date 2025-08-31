#include "tex.h"

#include <map>

std::map<unsigned char, Texture2D> loadedTextures;
std::map<char *, Texture2D> customKeyTextures;

unsigned char RegisterTexture(Image img)
{
    unsigned char newKey = loadedTextures.size() + 1;
    loadedTextures[newKey] = LoadTextureFromImage(img);

    return newKey;
}

unsigned char RegisterTexture(char *file)
{
    unsigned char newKey = loadedTextures.size() + 1;
    loadedTextures[newKey] = LoadTexture(file);

    return newKey;
}

char *RegisterTexture(Image img, char *customkey)
{
    customKeyTextures[customkey] = LoadTextureFromImage(img);
}

char *RegisterTexture(char *file, char *customkey)
{
    customKeyTextures[customkey] = LoadTexture(file);
}

Texture2D GetTexture(unsigned char key)
{
    return loadedTextures[key];
}

Texture2D GetTexture(char *key)
{
    return customKeyTextures[key];
}

void UnloadTextures()
{
    for (auto texture : loadedTextures)
    {
        UnloadTexture(texture.second);
        loadedTextures.erase(texture.first);
    }
}

void UnloadTexture(unsigned char key)
{
    loadedTextures.erase(key);
}

void UnloadTexture(char *key)
{
    customKeyTextures.erase(key);
}