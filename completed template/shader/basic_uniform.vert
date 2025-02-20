#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

flat out vec3 LightIntensity;

struct LightInfo {
    vec4 position;
    vec3 La; // Ambient light
    vec3 Ld; // Diffuse light
    vec3 Ls; // Specular light
};

struct MaterialInfo {
    vec3 Kd; // Diffuse reflectivity
    vec3 Ka; // Ambient reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};

uniform LightInfo Light;
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void getCamSpaceValues(out vec3 norm, out vec3 position)
{
    norm = normalize(NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    vec3 n, pos;
    getCamSpaceValues(n, pos);
    vec3 ambient = Light.La * Material.Ka;

    vec3 s = normalize(vec3(Light.position) - pos);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 specular = vec3(0.0);

    if(sDotN > 0.0)
    {
        vec3 v = normalize(-pos);
        vec3 r = reflect(-s, n);
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }

    LightIntensity = ambient + diffuse + specular;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}



