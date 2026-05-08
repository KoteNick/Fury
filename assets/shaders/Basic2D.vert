#version 330 core
        
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

uniform mat4 u_MVP;

out vec3 v_Color;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    v_Color = color;
};