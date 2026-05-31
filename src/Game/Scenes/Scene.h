//
// This file is meant to be copied to make a custom scene
//
#pragma once
#include "Game/BaseScene.h"

class Scene : public BaseScene {
public:
	void Init() override;
	void OnUpdate(float deltaTime) override;
};