#version 450 core
in vec3 v_LocalPos;
out vec4 FragColor;

uniform vec3 u_TopColor = vec3(0.1, 0.3, 0.6);
uniform vec3 u_BottomColor = vec3(0.7, 0.8, 0.9);

uniform vec3 u_LightDir; 

void main() {
    vec3 viewDir = normalize(v_LocalPos);
    
    float t = clamp(viewDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 finalColor = mix(u_BottomColor, u_TopColor, t);
    
    float sunDot = dot(viewDir, normalize(u_LightDir));
    
    if (sunDot > 0.9998) {
        finalColor = vec3(1.0, 1.0, 0.8) * 2.0;
    } 

    else if (sunDot > 0.998) {
        float glow = smoothstep(0.998, 0.9998, sunDot);
        finalColor += vec3(1.0, 0.8, 0.4) * glow; 
    }

    FragColor = vec4(finalColor, 1.0);
}