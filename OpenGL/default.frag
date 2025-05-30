#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D tex0;

uniform struct Light {
    vec3 position;
    vec3 color;
} light;

uniform vec3 viewPos; // Camera position

void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color;

    // Diffuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Shininess 32
    vec3 specular = specularStrength * spec * light.color;

    vec3 result = (ambient + diffuse + specular) * texture(tex0, texCoord).rgb;
    FragColor = vec4(result, 1.0);
}