#version 460

in vec3 FragPos;
in vec3 Normal;
in vec3 LightDir;
in vec2 TexCoord; // Receive texture coordinates from the vertex shader

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
uniform sampler2D Texture; // Add texture sampler

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

    // Apply texture
    vec3 texColor = texture(Texture, TexCoord).rgb;
    color *= texColor;

    // Fog calculations
    float distance = length(FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, color, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}

