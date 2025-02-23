#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord; // Add texture coordinate attribute

out vec3 FragPos;
out vec3 Normal;
out vec3 LightDir;
out vec2 TexCoord; // Pass texture coordinates to the fragment shader

struct LightInfo {
    vec4 position;
    vec3 La; // Ambient light
    vec3 Ld; // Diffuse light
    vec3 Ls; // Specular light
    vec3 direction; // Spotlight direction
    float cutoff; // Spotlight cutoff angle (in degrees)
};

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};

uniform LightInfo Light;
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    vec3 n = normalize(NormalMatrix * VertexNormal);
    vec4 viewPos = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(Light.position) - viewPos.xyz);

    FragPos = viewPos.xyz;
    Normal = n;
    LightDir = s;
    TexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y); // Flip the V coordinate

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
