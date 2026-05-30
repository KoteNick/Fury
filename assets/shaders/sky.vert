#version 450 core
layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};
uniform mat4 u_Model;

out vec3 v_LocalPos;

void main() {
    v_LocalPos = a_Position;
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0);
}