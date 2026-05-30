#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_FragPosLightSpace;

layout(std140, binding = 0) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

uniform sampler2D u_HeightMap;
uniform float u_HeightScale; 

void main()
{
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    vec2 uv = a_TexCoord;
    
    float height = texture(u_HeightMap, uv).r;
    worldPos.y += height * u_HeightScale;

    vec2 texelSize = 1.0 / textureSize(u_HeightMap, 0);

    float hL = texture(u_HeightMap, uv + vec2(-texelSize.x, 0.0)).r * u_HeightScale;
    float hR = texture(u_HeightMap, uv + vec2(texelSize.x, 0.0)).r * u_HeightScale;
    float hD = texture(u_HeightMap, uv + vec2(0.0, -texelSize.y)).r * u_HeightScale;
    float hU = texture(u_HeightMap, uv + vec2(0.0, texelSize.y)).r * u_HeightScale;

    vec3 tangent = vec3(0.5, hR - hL, 0.0);
    vec3 bitangent = vec3(0.0, hU - hD, 0.5);

    vec3 smoothNormal = normalize(cross(bitangent, tangent));

    v_Normal = mat3(u_Model) * smoothNormal;  
    
    v_FragPos = worldPos.xyz;
    v_TexCoord = uv;
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
    
    gl_Position = u_ProjectionView * worldPos;
}