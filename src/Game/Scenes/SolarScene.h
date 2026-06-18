#pragma once
#include "Game/BaseScene.h"

struct SolarObject {
    Entity* entity = nullptr;
    SolarObject* parent = nullptr;

    std::string name;

    float orbitDistance = 0.f;
    float orbitSpeed = 0.f;
    float currentOrbitAngle = 0.f;
    float orbitInclination = 0.f;

    float rotationSpeed = 0.f;
    float currentRotationAngle = 0.f;
    float axialTilt = 0.f;

    glm::vec3 worldPosition = glm::vec3(0.0f);
};

class SolarScene : public BaseScene {
    bool assetsLoaded = false;
public:

    std::vector<SolarObject> objects;
    float timeScale = 1.f;
    int timeScaleMode = 1;

    SolarObject* trackedObj = nullptr;
    float trackRadius = 15.0f;
    float trackYaw = 0.0f;
    float trackPitch = 20.0f;

    void LoadAssets();

    SolarObject& CreateSolarObject(const std::string& name);
    SolarObject& CreateSolarObject(const std::string& name, SolarObject* parent, float distance, float orbitSpeed, float rotationSpeed, float tilt, float scale);

	void Init() override;
	void OnUpdate(float deltaTime) override;
    void Destroy() override;
};