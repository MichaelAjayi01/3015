#version 460

layout(location = 0) in vec3 vertexPosition;
out vec3 Vec;

uniform mat4 MVP;

void main() {
    Vec = vertexPosition;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}
