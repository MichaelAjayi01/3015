#version 460

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;

out vec3 Position;
out vec2 TexCoord;
out vec3 T;
out vec3 B;
out vec3 N;
out float FogFactor;
out float FragY;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform float FogDensity;

void main()
{
    TexCoord = vertexTexCoord;

    vec3 normal = normalize(NormalMatrix * vertexNormal);
    vec3 tangent = normalize(NormalMatrix * vertexTangent);
    vec3 bitangent = normalize(cross(normal, tangent));

    N = normal;
    T = tangent;
    B = bitangent;

    Position = (ModelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    FragY = Position.y;

    float distance = length(Position);
    FogFactor = exp(-FogDensity * distance);
}
