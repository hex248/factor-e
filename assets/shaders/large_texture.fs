#version 330


// these come from vertex shader (.vs)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec2 fragWorldPos;

uniform sampler2D texture0;
uniform float tileSize;
uniform vec2 textureSize;
uniform vec2 tilesInTexture;

out vec4 finalColor;

void main()
{
    vec2 tilePos = floor(fragWorldPos / tileSize);
    
    vec2 positionInTile = fract(fragWorldPos / tileSize);
    
    // map current tile position to texture coordinates
    vec2 tileUV = mod(tilePos, tilesInTexture) / tilesInTexture;
    
    vec2 finalUV = tileUV + (positionInTile / tilesInTexture);
    
    vec4 texelColor = texture(texture0, finalUV);
    
    finalColor = texelColor * fragColor;
}
