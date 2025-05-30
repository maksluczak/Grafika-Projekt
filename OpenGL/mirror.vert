#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec4 vertexColor; // For the mirror surface color
out vec2 texCoord;

uniform mat4 model;
uniform mat4 camMatrix;
uniform vec4 mirrorColor; // Uniform for the mirror's color and alpha

void main() {
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
    vertexColor = mirrorColor;
    texCoord = aTexCoord;
}