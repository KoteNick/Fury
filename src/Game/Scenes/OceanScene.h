#pragma once
#include "Game/BaseScene.h"

struct OceanUBOData {
    glm::vec4 Ambient = glm::vec4(0.0f, 0.05f, 0.1f, 1.0f);
    glm::vec4 DiffuseReflectance = glm::vec4(0.0f, 0.15f, 0.25f, 1.0f);
    glm::vec4 SpecularReflectance = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 FresnelColor = glm::vec4(0.1f, 0.4f, 0.5f, 1.0f);
    glm::vec4 TipColor = glm::vec4(0.8f, 0.9f, 1.0f, 1.0f);

    float Frequency = 0.2f;       
    float FrequencyMult = 1.18f;  
    float Amplitude = 1.0f;       
    float AmplitudeMult = 0.82f;  

    float InitialSpeed = 1.5f;    
    float SpeedRamp = 1.07f;      
    float Drag = 0.05f;
    float Height = 1.5f;  

    float MaxPeak = 1.5f; 
    float PeakOffset = 1.0f;
    float Seed = 0.0f;
    float SeedIter = 1253.2f;

    float Shininess = 256.0f;     
    float FresnelBias = 0.05f;    
    float FresnelStrength = 1.0f; 
    float FresnelShininess = 5.0f;

    float TipAttenuation = 4.0f;
    float NormalStrength = 1.0f;
    int WaveCount = 16; 
    float padding = 0.0f;
};

class OceanScene : public BaseScene {
public:
    float time = 0;
    OceanUBOData oceanData;
    Entity *sky, *sun;
    int sunSize = 2;
    glm::vec3 skyTop = { 0.1, 0.3, 0.6 }, skyBot = { 0.7, 0.8, 0.9 };

	void Init() override;
	void OnUpdate(float deltaTime) override;
};