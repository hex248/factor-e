#include "raylib.h"

unsigned char RegisterTexture(Image img);
unsigned char RegisterTexture(char *file);
char *RegisterTexture(Image img, char *customkey);
char *RegisterTexture(char *file, char *customkey);

Texture2D GetTexture(unsigned char key);
Texture2D GetTexture(char *key);

void UnloadTextures();
void UnloadTexture(unsigned char key);
void UnloadTexture(char *key);