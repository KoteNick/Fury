//
// This file is meant to be copied to make a custom scene
//
#pragma once
#include "Game/BaseScene.h"

class SolarScene : public BaseScene {
public:
	void Init() override;
	void OnUpdate(float deltaTime) override;
};