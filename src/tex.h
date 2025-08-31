#include "raylib.h"

unsigned char RegisterTexture(Image img);
unsigned char RegisterTexture(const char *file);
char *RegisterTexture(Image img, const char *customkey);
char *RegisterTexture(char *file, const char *customkey);

Texture2D GetTexture(unsigned char key);
Texture2D GetTexture(const char *key);

void UnloadTextures();
void UnloadTexture(unsigned char key);
void UnloadTexture(const char *key);