#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform CameraUBO {
    mat4 u_ProjectionView;
    vec4 u_ViewPos;
};

layout(std140) uniform SunUBO {
    vec4 u_SunDirInt;
    vec4 u_SunDiskColorAndSize;
    vec4 u_SunLightColor;
    vec4 u_SunSpecColor;
};

uniform mat4 u_Model;
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

out vec3 v_FragPos;
out vec2 v_TexCoord;

// Генерация волн
vec3 vertexFBM(vec3 v) {
    float f = u_Frequency;
    float a = u_Amplitude;
    float speed = u_InitialSpeed;
    float seed = u_Seed;
    vec3 p = v;
    float amplitudeSum = 0.0;
    
    float h = 0.0;
    vec2 n = vec2(0.0);
    
    int count = min(u_WaveCount, 32);
    
    for (int wi = 0; wi < count; ++wi) {
        vec2 d = normalize(vec2(cos(seed), sin(seed)));

        float x = dot(d, p.xz) * f + u_Time * speed;
        float wave = a * exp(u_MaxPeak * sin(x) - u_PeakOffset);
        float dx = u_MaxPeak * wave * cos(x);
        
        h += wave;
        
        p.xz += d * -dx * a * u_Drag;

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
    vec3 worldPos = (u_Model * vec4(a_Position, 1.0)).xyz;
    
    vec3 fbm = vertexFBM(worldPos);
    worldPos.y += fbm.x;

    v_FragPos = worldPos;
    v_TexCoord = a_TexCoord;
    gl_Position = u_ProjectionView * vec4(worldPos, 1.0);
}