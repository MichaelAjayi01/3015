#version 130

in vec3 Position;
in vec2 TexCoord;
in vec3 T;
in vec3 B;
in vec3 N;
in float FragY;

uniform sampler2D MainText;
uniform sampler2D NormalText;
out vec4 FragColor;

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
uniform vec3 CameraPosition;
uniform float Time; // Time for animation

// Simple noise function
float random(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Smooth noise function for better results
float smoothNoise(vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

vec3 BlinnphongSpot(vec3 position, vec3 n) {
    vec3 diffuse = vec3(0), specular = vec3(0);

    vec3 mainColor = texture2D(MainText, TexCoord).rgb;

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
    vec3 normalMap = texture2D(NormalText, TexCoord).rgb;
    normalMap.g = 1.0 - normalMap.g;
    normalMap = normalize(normalMap * 2.0 - 1.0);
    normalMap = mix(vec3(0.0, 0.0, 1.0), normalMap, 0.5);

    // Transform normal from tangent space to world space
    vec3 T_normalized = normalize(T);
    vec3 B_normalized = normalize(B);
    vec3 N_normalized = normalize(N);
    mat3 TBN = mat3(T_normalized, B_normalized, N_normalized);
    vec3 mappedNormal = normalize(TBN * normalMap);

    vec3 color = BlinnphongSpot(Position, mappedNormal);

    // Fresnel Transparency
    vec3 viewDir = normalize(CameraPosition - Position);
    float fresnel = pow(1.0 - max(dot(mappedNormal, viewDir), 0.0), 3.0);
    float alpha = mix(0.7, 1.0, fresnel);

    // Noise-Based Fragment Discard (Shimmer Effect)
    float noise = smoothNoise(TexCoord * 10.0 + vec2(Time * 0.5, Time * 0.3)); 
    float threshold = mix(0.3, 0.7, fresnel); // More stable edges, fluctuating center
    if (noise < threshold) {
        discard; // Removes some pixels to create the flickering effect
    }

    // Fog Effect
    float fogFactor;
    float fogStartY = -20.0; // Decrease this value to start fog closer
    float fogEndY = 10.0; // Increase this value to end fog further

    if (FragY > fogEndY) {
        fogFactor = 1.0;
    } else if (FragY > fogStartY) {
        fogFactor = mix(1.0, 0.0, (fogEndY - FragY) / (fogEndY - fogStartY));
    } else {
        fogFactor = 0.0;
    }

    vec3 finalColor = mix(fogColor, color, fogFactor);
    FragColor = vec4(finalColor, alpha);
}