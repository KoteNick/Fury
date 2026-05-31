#pragma once
#include "Game/BaseScene.h"

struct TerrainUBOData {
    glm::vec4 LowSlopeColor = glm::vec4(0.83f, 0.88f, 0.94f, 1.0f);
    glm::vec4 HighSlopeColor = glm::vec4(0.16f, 0.1f, 0.1f, 1.0f);
    glm::vec4 AmbientLight = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    glm::vec3 Offset = glm::vec3(0.0f);
    float Seed = 0.0f;

    glm::vec2 AngularVariance = glm::vec2(0.0f, 0.0f);
    glm::vec2 SlopeRange = glm::vec2(0.9f, 0.98f);

    float GradientRotation = 0.0f;
    float NoiseRotation = 30.0f;
    float TerrainHeight = 50.0f;
    float Scale = 100.0f;

    float Octaves = 10.0f;
    float AmplitudeDecay = 0.45f;
    float NormalStrength = 1.0f;
    float InitialAmplitude = 0.5f;

    float Lacunarity = 2.0f;
    float FrequencyVarianceLowerBound = 0.0f;
    float FrequencyVarianceUpperBound = 0.0f;
    float SlopeDamping = 0.2f;
};

class TerrainScene : public BaseScene {
public:
    Entity* sky;
    TerrainUBOData terrainSettings;
    SunUBOData sunData;

	void Init() override;
	void OnUpdate(float deltaTime) override;
};