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
    vec3 lightDir = normalize(vec3(0.0, 3.0, 2.0) - fragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec4 baseColor = texture(tex0, texCoord);

    if (useOverrideColor)
    {
        FragColor = vec4(overrideColor.rgb * baseColor.rgb * diff, 0.5);
    }
    else
    {
        FragColor = vec4(baseColor.rgb * diff, 1.0);
    }
}
