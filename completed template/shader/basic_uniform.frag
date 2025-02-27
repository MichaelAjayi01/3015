#version 460

in vec3 Position;
in vec2 TexCoord;
in vec3 T;
in vec3 B;
in vec3 N;
in float FragY;

layout(binding = 0) uniform sampler2D MainText;
layout(binding = 1) uniform sampler2D NormalText;
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
    // Retrieve normal from normal map
    vec3 normalMap = texture(NormalText, TexCoord).rgb;
    normalMap.g = 1.0 - normalMap.g; // Flip the green channel
    normalMap = normalize(normalMap * 2.0 - 1.0); // Convert from [0,1] to [-1,1]
    normalMap = mix(vec3(0.0, 0.0, 1.0), normalMap, 0.5); // Reduce strength

    // Transform normal from tangent space to world space
    vec3 T_normalized = normalize(T);
    vec3 B_normalized = normalize(B);
    vec3 N_normalized = normalize(N);
    mat3 TBN = mat3(T_normalized, B_normalized, N_normalized);
    vec3 mappedNormal = normalize(TBN * normalMap);

    vec3 color = BlinnphongSpot(Position, mappedNormal);

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

    vec3 finalColor = mix(fogColor, color, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
