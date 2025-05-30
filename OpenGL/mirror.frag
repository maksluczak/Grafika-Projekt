#version 330 core

out vec4 FragColor;

in vec4 vertexColor; // Receives the mirror color from vertex shader
in vec2 texCoord;

void main() {
    FragColor = vertexColor;
}