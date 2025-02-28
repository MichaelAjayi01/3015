#version 460

layout(binding = 0) uniform samplerCube SkyBoxTex;
in vec3 Vec;
layout(location = 0) out vec4 FragColor;

void main() {
    vec3 textColor = texture(SkyBoxTex, normalize(Vec)).rgb;
    FragColor = vec4(textColor, 1.0);
}
