#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;

// these go to fragment shader (.fs)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec2 fragWorldPos;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    
    // calculate world position for texture sampling using the model matrix to get world coords
    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);
    fragWorldPos = worldPos.xy;
    
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
