#version 460

in vec3 Position;
in vec3 Normal;
in float FragY; 
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D MainText;
layout(location = 0) out vec4 FragColor;

struct SpotLightInfo {
    vec3 Position;
    vec3 La;
    vec3 L;
    vec3 Direction;
    float Exponent;
    float Cutoff;
};

struct MaterialInfo {
    vec3 Kd; 
    vec3 Ka; 
    vec3 Ks; 
    float Shininess;
};

uniform SpotLightInfo Spot;
uniform MaterialInfo Material;
uniform vec3 fogColor;

vec3 BlinnphongSpot(vec3 position, vec3 n) {
    vec3 diffuse = vec3(0), specular = vec3(0);

    vec3 mainColor = texture(MainText, TexCoord).rgb;

    vec3 ambient = Spot.La * Material.Ka * mainColor;
    vec3 s = normalize(Spot.Position - position);

    float cosAngle = dot(-s, Spot.Direction);
    float angle = acos(cosAngle);
    
    if (angle >= 0.0 && angle < Spot.Cutoff) {
        float spotScale = pow(cosAngle, Spot.Exponent);
        float sDotN = max(dot(s, n), 0.0);
        diffuse = mainColor * sDotN;

        if (sDotN > 0.0) {
            vec3 v = normalize(-position);
            vec3 h = normalize(v + s);
            specular = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
        }

        return ambient + spotScale * (diffuse + specular) * Spot.L;
    }

    return ambient;
}

void main() {
    vec3 color = BlinnphongSpot(Position, normalize(Normal));
    
    // Apply fog based on the y-coordinate
    float fogFactor;
    float fogStartY = -20.0;
    float fogEndY = -10.0;

    if (FragY > fogEndY) {
        fogFactor = 1.0;
    } else if (FragY > fogStartY) {
        fogFactor = mix(1.0, 0.0, (fogEndY - FragY) / (fogEndY - fogStartY));
    } else {
        fogFactor = 0.0;
    }

    // Blend the fog color with the Blinn-Phong color
    vec3 finalColor = mix(fogColor, color, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
