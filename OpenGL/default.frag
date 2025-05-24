#version 330 core

in vec3 crntColor;
in vec2 texCoord;
in vec3 Normal;
in vec3 fragPos;

uniform sampler2D tex0;
uniform vec4 overrideColor;
uniform bool useOverrideColor = false;

out vec4 FragColor;

void main()
{
    vec4 baseColor = texture(tex0, texCoord);
    if (useOverrideColor)
    {
        // Simulacja rozmycia i pó³przezroczystoœci odbicia
        FragColor = vec4(overrideColor.rgb * baseColor.rgb, 0.5);
    }
    else
    {
        FragColor = baseColor;
    }
}