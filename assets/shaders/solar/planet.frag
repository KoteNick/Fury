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

uniform mat4 u_Model;
uniform vec4 u_Color;
uniform float u_Shininess;
uniform int u_IsAtmosphere;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_EmissionMap;

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).xyz * 2.0 - 1.0;
    
    tangentNormal.xy *= 3.0;
    tangentNormal = normalize(tangentNormal);

    vec3 localPos = (inverse(u_Model) * vec4(v_FragPos, 1.0)).xyz;
    
    vec3 localN = normalize(localPos);

    vec3 localT = vec3(localN.z, 0.0, -localN.x);
    if (length(localT) < 0.001) {
        localT = vec3(1.0, 0.0, 0.0);
    } else {
        localT = normalize(localT);
    }

    vec3 N = normalize(v_Normal);
    vec3 T = normalize(mat3(u_Model) * localT);
    
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

void main()
{
    vec4 albedo = texture(u_DiffuseMap, v_TexCoord) * u_Color;
    float specularMask = texture(u_SpecularMap, v_TexCoord).r;
    vec3 nightLights = texture(u_EmissionMap, v_TexCoord).rgb;

    vec3 normal = GetNormalFromMap();
    vec3 viewDir = normalize(u_ViewPos.xyz - v_FragPos);

    float finalAlpha = albedo.a;

    if (u_IsAtmosphere == 1) 
    {
        float brightnessMask = max(albedo.r, max(albedo.g, albedo.b));
    
        vec3 baseNormal = normalize(v_Normal);
        float fresnel = 1.0 - max(dot(baseNormal, viewDir), 0.0);
        fresnel = pow(fresnel, 3.0);
    
        float cloudAlpha = brightnessMask * albedo.a;
    
        finalAlpha = clamp(cloudAlpha + fresnel, 0.0, 1.0);
    
        albedo.rgb += albedo.rgb * fresnel;
    }

    vec3 ambient = u_GlobalAmbient.xyz;
    vec3 totalLight = vec3(0.0);
    
    float sunIllumination = 0.0; 
    
    int activeLights = min(u_LightCount, MAX_LIGHTS);
    for(int i = 0; i < activeLights; i++)
    {
        vec3 lightPosOrDir = u_Lights[i].positionRadius.xyz;
        vec3 lightColor = u_Lights[i].colorIntensity.xyz;
        float intensity = u_Lights[i].colorIntensity.w;

        vec3 lightDir = normalize(lightPosOrDir - v_FragPos);

        float NdotL = dot(normal, lightDir);
        float diff = max(NdotL, 0.0);
        vec3 diffuse = diff * lightColor;

        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
        
        vec3 specular = spec * lightColor * specularMask; 

        totalLight += (diffuse + specular) * intensity;

        sunIllumination += smoothstep(-0.2, 0.2, NdotL);
    }

    vec3 finalEmission = nightLights * clamp(1.0 - sunIllumination, 0.0, 1.0);

    vec3 result = (ambient + totalLight) * albedo.rgb + finalEmission;

    FragColor = vec4(result, finalAlpha);
}