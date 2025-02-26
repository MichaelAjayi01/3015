#version 460

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out float FogFactor;
out float FragY;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform float FogDensity; // Uniform for fog density

void main()
{
    TexCoord = vertexTexCoord;

    Normal = normalize(NormalMatrix * vertexNormal);
    Position = (ModelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass the y-coordinate to the fragment shader
    FragY = Position.y;

    // Calculate fog factor
    float distance = length(Position);
    FogFactor = exp(-FogDensity * distance);
}
