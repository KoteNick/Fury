#version 450 core
in vec3 v_FragPos;
in vec2 v_TexCoord;
out vec4 FragColor;

layout(std140) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};

layout(std140) uniform SunUBO {
    vec4 u_SunDirInt;
    vec4 u_SunDiskColorAndSize;
    vec4 u_SunLightColor;
    vec4 u_SunSpecColor;
};

struct LightData { vec4 positionRadius; vec4 colorIntensity; };
#define MAX_LIGHTS 8
layout (std140) uniform LightsUBO {
    int u_LightCount;
    LightData u_Lights[MAX_LIGHTS];
};

uniform float u_Time;

layout(std140, binding = 8) uniform OceanUBO {
    vec4 u_Ambient; vec4 u_DiffuseReflectance; vec4 u_SpecularReflectance;
    vec4 u_FresnelColor; vec4 u_TipColor;
    float u_Frequency; float u_FrequencyMult; float u_Amplitude; float u_AmplitudeMult;
    float u_InitialSpeed; float u_SpeedRamp; float u_Drag; float u_Height;
    float u_MaxPeak; float u_PeakOffset; float u_Seed; float u_SeedIter;
    float u_Shininess; float u_FresnelBias; float u_FresnelStrength; float u_FresnelShininess;
    float u_TipAttenuation; float u_NormalStrength; int u_WaveCount;
};

vec3 fragmentFBM(vec3 v) {
    float f = u_Frequency;
    float a = u_Amplitude;
    float speed = u_InitialSpeed;
    float seed = u_Seed;
    vec3 p = v;

    float h = 0.0;
    vec2 n = vec2(0.0);
    float amplitudeSum = 0.0;
    int count = min(u_WaveCount, 16);

    for (int wi = 0; wi < count; ++wi) {
        vec2 d = normalize(vec2(cos(seed), sin(seed)));

        float x = dot(d, p.xz) * f + u_Time * speed;
        float wave = a * exp(u_MaxPeak * sin(x) - u_PeakOffset);
        
        vec2 dw = f * d * (u_MaxPeak * wave * cos(x));
        
        h += wave;
        p.xz += -dw * a * u_Drag;
        n += dw;
        
        amplitudeSum += a;
        f *= u_FrequencyMult;
        a *= u_AmplitudeMult;
        speed *= u_SpeedRamp;
        seed += u_SeedIter;
    }

    vec3 output_val = vec3(h, n.x, n.y) / max(amplitudeSum, 0.001);
    output_val.x *= u_Height;
    return output_val;
}

void main() {
    vec3 fbm = fragmentFBM(v_FragPos);
    float height = fbm.x; 
    
    vec3 normal = normalize(vec3(-fbm.y, 1.0, -fbm.z));
    normal.xz *= u_NormalStrength;
    normal = normalize(normal);

    vec3 viewDir = normalize(u_ViewPos.xyz - v_FragPos);
    
    vec3 sunDir = normalize(-u_SunDirInt.xyz);
    float sunIntensity = max(u_SunDirInt.w, 0.0);
    
    float sunNdotL = max(dot(sunDir, normal), 0.0);
    vec3 sunDiffuse = u_SunLightColor.rgb * sunNdotL * sunIntensity * (u_DiffuseReflectance.rgb / 3.14159);
    
    vec3 halfwayDir = normalize(sunDir + viewDir);
    float specPower = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 sunSpecular = u_SunSpecColor.rgb * specPower * sunIntensity * u_SpecularReflectance.rgb;

    vec3 localDiffuse = vec3(0.0);
    vec3 localSpecular = vec3(0.0);
    
    int activeLights = min(u_LightCount, MAX_LIGHTS);
    for(int i = 0; i < activeLights; i++) {
        LightData light = u_Lights[i];
        vec3 lightPosOrDir = light.positionRadius.xyz;
        float radius = light.positionRadius.w;
        vec3 lightColor = light.colorIntensity.xyz;
        float intensity = light.colorIntensity.w;

        if (radius > 0.0) {
            vec3 lightDir = lightPosOrDir - v_FragPos;
            float distance = length(lightDir);
            
            if (distance < radius) {
                lightDir = normalize(lightDir);
                float attenuation = clamp(1.0 - (distance * distance) / (radius * radius), 0.0, 1.0);
                attenuation *= attenuation; 

                float diff = max(dot(normal, lightDir), 0.0);
                localDiffuse += lightColor * diff * attenuation * intensity * (u_DiffuseReflectance.rgb / 3.14159);

                vec3 localHalfDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(normal, localHalfDir), 0.0), u_Shininess);
                localSpecular += lightColor * spec * attenuation * intensity * u_SpecularReflectance.rgb;
            }
        }
    }
    
    float base = 1.0 - clamp(dot(viewDir, normal), 0.0, 1.0);
    float R = pow(base, u_FresnelShininess) * u_FresnelStrength;
    vec3 fresnel = u_FresnelColor.rgb * R;
    
    sunSpecular *= (R + 0.5); 
    localSpecular *= (R + 0.5);

    vec3 tipColor = u_TipColor.rgb * pow(clamp(height / u_Height, 0.0, 1.0), u_TipAttenuation);

    vec3 finalColor = u_Ambient.rgb + sunDiffuse + sunSpecular + localDiffuse + localSpecular + fresnel + tipColor;
    
    vec3 a = vec3(2.51); vec3 b = vec3(0.03); vec3 c = vec3(2.43); vec3 d = vec3(0.59); vec3 e = vec3(0.14);
    finalColor = clamp((finalColor * (a * finalColor + b)) / (finalColor * (c * finalColor + d) + e), 0.0, 1.0);
    
    FragColor = vec4(finalColor, 0.95);
}