#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;

out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_DiffuseMap;

void main()
{
    vec4 texColor = texture(u_DiffuseMap, v_TexCoord);
    color = u_Color * texColor;
}