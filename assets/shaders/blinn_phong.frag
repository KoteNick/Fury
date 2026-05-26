#version 450 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

struct LightData {
    vec4 positionRadius;
    vec4 colorIntensity;
};

#define MAX_LIGHTS 8

layout (std140, binding = 2) uniform LightsUBO {
    int u_LightCount;
    LightData u_Lights[MAX_LIGHTS];
};

layout (std140, binding = 1) uniform AmbientUBO {
    vec4 u_GlobalAmbient;
};

layout(std140, binding = 0) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};

uniform vec4 u_Color;
uniform float u_Shininess;

vec3 CalculateLight(LightData light, vec3 normal, vec3 viewDir)
{
    vec3 lightPosOrDir = light.positionRadius.xyz;
    float radius = light.positionRadius.w;
    
    vec3 lightColor = light.colorIntensity.xyz;
    float intensity = light.colorIntensity.w;

    vec3 lightDir;
    float attenuation = 1.0;

    if (radius < 0.0) 
    {
        lightDir = normalize(-lightPosOrDir);
        attenuation = 1.0;
    } 
    else 
    {
        lightDir = lightPosOrDir - v_FragPos;
        float distance = length(lightDir);
        
        if (distance > radius) return vec3(0.0);
        
        lightDir = normalize(lightDir);

        attenuation = clamp(1.0 - (distance * distance) / (radius * radius), 0.0, 1.0);
        attenuation *= attenuation; 
    }

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 specular = 0.5 * spec * lightColor;

    return (diffuse + specular) * attenuation * intensity;
}

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos.xyz - v_FragPos);

    vec3 ambient = u_GlobalAmbient.xyz;

    vec3 totalLight = vec3(0.0);
    
    int activeLights = min(u_LightCount, MAX_LIGHTS);
    for(int i = 0; i < activeLights; i++)
    {
        totalLight += CalculateLight(u_Lights[i], normal, viewDir);
    }

    vec3 result = (ambient + totalLight) * u_Color.rgb;
    
    FragColor = vec4(result, u_Color.a);
}