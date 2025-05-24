#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
}