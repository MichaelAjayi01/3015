#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightDir;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform vec4 LightPosition;

void main()
{
    vec3 n = normalize(NormalMatrix * VertexNormal);
    vec4 viewPos = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(LightPosition) - viewPos.xyz);

    FragPos = viewPos.xyz;
    Normal = n;
    LightDir = s;

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}


