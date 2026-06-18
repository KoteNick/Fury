#pragma once
#include "Game/BaseScene.h"

class FieldScene : public BaseScene {
	std::unique_ptr<Texture> heightmap;
	glm::vec3 fieldColor = {0.28, 0.43, 0.22};

	void GenerateHeightmapTexture(int width, int height, int seed = 0);
public:
	void Init() override;
	void OnUpdate(float deltaTime) override;
};