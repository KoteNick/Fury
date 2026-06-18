#version 450 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec4 v_FragPosLightSpace;

out vec4 FragColor;

struct LightData {
    vec4 positionRadius;
    vec4 colorIntensity;
};

#define MAX_LIGHTS 8

layout (std140) uniform LightsUBO {
    int u_LightCount;
    LightData u_Lights[MAX_LIGHTS];
};

layout (std140) uniform AmbientUBO {
    vec4 u_GlobalAmbient;
};

layout(std140) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};

uniform vec4 u_Color;
uniform float u_Shininess;
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_ShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

vec3 CalculateLight(LightData light, vec3 normal, vec3 viewDir)
{
    vec3 lightPosOrDir = light.positionRadius.xyz;
    float radius = light.positionRadius.w;
    
    vec3 lightColor = light.colorIntensity.xyz;
    float intensity = light.colorIntensity.w;

    vec3 lightDir;
    float attenuation = 1.0;
    float shadow = 0.0;

    if (radius < 0.0) 
    {
        lightDir = normalize(-lightPosOrDir);
        attenuation = 1.0; 
        
        shadow = ShadowCalculation(v_FragPosLightSpace, normal, lightDir);
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

    return (1.0 - shadow) * (diffuse + specular) * attenuation * intensity;
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

    vec4 texColor = texture(u_DiffuseMap, v_TexCoord);

    vec4 albedo = texColor * u_Color;

    vec3 result = (ambient + totalLight) * albedo.rgb;
    
    FragColor = vec4(result, albedo.a);
}