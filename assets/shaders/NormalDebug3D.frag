#version 330 core

in vec3 v_Normal;
out vec4 color;

uniform vec4 u_Color;

void main()
{
    vec3 Color = v_Normal * 0.5 + 0.5;
    
    color = vec4(Color, 1.0) * u_Color;
}