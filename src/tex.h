#include "raylib.h"
#include <string>

unsigned char RegisterTexture(Image img);
unsigned char RegisterTexture(const char *file);
std::string RegisterTexture(Image img, std::string customkey);
std::string RegisterTexture(const char *file, std::string customkey);

Texture2D GetTexture(unsigned char key);
Texture2D GetTexture(std::string customKey);

void UnregisterTextures();
void UnregisterTexture(unsigned char key);
void UnregisterTexture(std::string customKey);