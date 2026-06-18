#version 450 core

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

out vec4 FragColor;

uniform sampler2D u_DiffuseMap;

// “вои UBO со светом
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

void main()
{
    vec2 centeredUV = v_TexCoord * 2.0 - 1.0;
    float dist = length(centeredUV);

    float innerRad = 0.50; 
    float outerRad = 0.95; 

    if (dist > outerRad || dist < innerRad) {
        discard;
    }

    float u = (outerRad - dist) / (outerRad - innerRad);
    
    u = clamp(u, 0.0, 1.0); 

    vec4 texColor = textureLod(u_DiffuseMap, vec2(u, 0.5), 0.0);

    float alpha = texColor.a;

    if (alpha < 0.05) {
        discard; 
    }
    vec3 N = normalize(v_Normal);
    vec3 ambient = u_GlobalAmbient.xyz;
    vec3 totalDiffuse = vec3(0.0);

    if (u_LightCount > 0) 
    {
        vec3 lightPos = u_Lights[0].positionRadius.xyz;
        vec3 lightColor = u_Lights[0].colorIntensity.xyz;
        float intensity = u_Lights[0].colorIntensity.w;

        vec3 lightDir = normalize(lightPos - v_FragPos);

        float diff = abs(dot(N, lightDir)); 
        diff = diff * 0.6 + 0.4; 
        
        totalDiffuse = diff * lightColor * intensity;
    }

    vec3 resultColor = texColor.rgb * (ambient + totalDiffuse);

    FragColor = vec4(resultColor, alpha);
}