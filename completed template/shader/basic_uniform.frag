#version 460

in vec3 FragPos;
in vec3 Normal;
in vec3 LightDir;

out vec4 FragColor;

struct LightInfo {
    vec3 La; // Ambient light
    vec3 Ld; // Diffuse light
    vec3 Ls; // Specular light
};

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};

uniform LightInfo Light;
uniform MaterialInfo Material;

void main()
{
    vec3 ambient = Light.La * Material.Ka;

    vec3 n = normalize(Normal);
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;

    vec3 v = normalize(-FragPos);
    vec3 r = reflect(-s, n);
    float rDotV = max(dot(r, v), 0.0);
    vec3 specular = Light.Ls * Material.Ks * pow(rDotV, Material.Shininess);

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}


