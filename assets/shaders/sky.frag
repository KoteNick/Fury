#version 450 core
in vec3 v_LocalPos;
out vec4 FragColor;

layout(std140) uniform SunUBO {
    vec4 u_SunDirInt;
    vec4 u_SunDiskColorAndSize;
    vec4 u_SunLightColor;
    vec4 u_SunSpecColor;
};

uniform vec3 u_TopColor = vec3(0.1, 0.3, 0.6);
uniform vec3 u_BottomColor = vec3(0.7, 0.8, 0.9);

void main() {
    vec3 viewDir = normalize(v_LocalPos);
    
    // Градиент неба
    float t = clamp(viewDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 finalColor = mix(u_BottomColor, u_TopColor, t);
    
    // --- РИСУЕМ СОЛНЦЕ ---
    // Узнаем направление К солнцу (инвертируем u_SunDirInt)
    vec3 sunPosDir = normalize(-u_SunDirInt.xyz);
    float sunDot = dot(viewDir, sunPosDir);
    
    vec3 sunColor = u_SunDiskColorAndSize.rgb;
    float sunSize = u_SunDiskColorAndSize.w; 

    // Четкий диск
    if (sunDot > sunSize) {
        finalColor = sunColor * u_SunDirInt.w; 
    } 
    // Мягкий ореол (Glow)
    else if (sunDot > sunSize - 0.02) {
        float glow = smoothstep(sunSize - 0.02, sunSize, sunDot);
        finalColor += sunColor * glow * (u_SunDirInt.w * 0.5);
    }

    FragColor = vec4(finalColor, 1.0);
}