#pragma once
#include "Game/BaseScene.h"

class TestScene : public BaseScene {
public:
	void Init() override;
	void OnUpdate(float deltaTime) override;
};