#version 460

flat in vec3 FragPos;
flat in vec3 Normal;
flat in vec3 LightDir;

out vec4 FragColor;

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

// Fog parameters
uniform vec3 fogColor;
uniform float fogDensity;

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

    // Spotlight effect
    float theta = dot(s, normalize(-Light.direction));
    float epsilon = Light.cutoff - 0.1;
    float intensity = clamp((theta - epsilon) / (Light.cutoff - epsilon), 0.0, 1.0);

    vec3 color = ambient + (diffuse + specular) * intensity;

    // Fog calculations
    float distance = length(FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, color, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
