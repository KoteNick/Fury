#pragma once
#include "Core/Entity.h"
#include "Scenes/Scenes.h"
#include "BaseScene.h"

#include <vector>

class Game {
	struct WindowSize {
		int width, height;
	};
private:
	static Game* instance;
	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes;
	BaseScene* currentScene = nullptr;
public:
	Game() {
		instance = this;
	}
	~Game() {
		instance = nullptr;
	}
	static Game& Get() { return *instance; }
public:
	bool debugUI = false, shadowPass = true;
	float time = 0;
	WindowSize windowSize = {};

	void Init();
	void OnUpdate(float deltaTime);
	void Close();
};